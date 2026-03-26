#include "GAS/Ability/Enemy/LRGA_EnemyAreaAttack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "System/LoggingSystem.h"
#include "Units/LRCharacter.h"

const FName ULRGA_EnemyAreaAttack::HitNotifyName = TEXT("BasicHit");

ULRGA_EnemyAreaAttack::ULRGA_EnemyAreaAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Skill_EnemyAreaAttack);
	SetAssetTags(TempTags);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_EnemyAreaAttack;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_ENEMY_AREA_ATTACK";
	SkillEffectID = "EFFECT_ENEMY_AREA_ATTACK";

	CooldownTagContainer.AddTag(
		FGameplayTag::RequestGameplayTag(FName("Ability.Skill.EnemyAreaAttack.Cooldown")));
}

void ULRGA_EnemyAreaAttack::OnAbilityActivated(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	bDamageApplied = false;
	ActiveMontage = nullptr;

	if (!DamageEffectClass)
	{
		LR_WARN(TEXT("[EnemyAreaAttack] DamageEffectClass 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay = Cast<UAnimMontage>(const_cast<UObject*>(CachedOptionalObject.Get()));
	if (!MontageToPlay)
	{
		LR_WARN(TEXT("[EnemyAreaAttack] 몽타주 없음 — 즉시 범위 데미지 적용"));
		ApplyAreaDamage();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*ActorInfo);
	UAnimInstance* AnimInstance = (OwnerChar && OwnerChar->GetMesh()) ? OwnerChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		LR_WARN(TEXT("[EnemyAreaAttack] AnimInstance 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	float MontageLength = AnimInstance->Montage_Play(MontageToPlay, 1.f);
	if (MontageLength <= 0.f)
	{
		LR_WARN(TEXT("[EnemyAreaAttack] 몽타주 재생 실패"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ActiveMontage = MontageToPlay;

	// 풀링 안전 델리게이트 바인딩
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ULRGA_EnemyAreaAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ULRGA_EnemyAreaAttack::OnMontageNotifyBegin);
	AnimInstance->OnMontageEnded.AddDynamic(this, &ULRGA_EnemyAreaAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ULRGA_EnemyAreaAttack::OnMontageNotifyBegin);
}

void ULRGA_EnemyAreaAttack::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	ALRCharacter* OwnerChar = CurrentActorInfo ? GetCharacterFromActorInfo(*CurrentActorInfo) : nullptr;
	UAnimInstance* AnimInstance = (OwnerChar && OwnerChar->GetMesh()) ? OwnerChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		LR_WARN(TEXT("[EnemyAreaAttack] <OnMontageNotifyBegin()> AnimInstance 없음"));
		return;
	}

	FAnimMontageInstance* MontageInstance = AnimInstance->GetMontageInstanceForID(Payload.MontageInstanceID);
	if (!MontageInstance || MontageInstance->Montage != ActiveMontage || NotifyName != HitNotifyName)
	{
		return;
	}
	if (bDamageApplied) return;
	bDamageApplied = true;

	ApplyAreaDamage();
}

void ULRGA_EnemyAreaAttack::ApplyAreaDamage()
{
	ALRCharacter* OwnerChar = CurrentActorInfo ?
		GetCharacterFromActorInfo(*CurrentActorInfo) : nullptr;
	if (!OwnerChar) return;

	UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC) return;

	// 공격 판정 중심
	FVector StrikeCenter = OwnerChar->GetActorLocation() + OwnerChar->GetActorForwardVector() * StrikeDistance;

	// ── 범위 내 액터 수집 ──
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerChar);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), StrikeCenter, HitRadius,
		ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), StrikeCenter, HitRadius, 16, FColor::Orange, false, 1.0f);
#endif

	// ── 적대 대상에 GE 적용 ──
	FGameplayTag HostileTag = GetHostileTeamTag();

	for (AActor* HitActor : OutActors)
	{
		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!TargetASC) continue;
		if (!TargetASC->HasMatchingGameplayTag(HostileTag)) continue;
		if (TargetASC->HasMatchingGameplayTag(LRTags::State_Dead)) continue;

		FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
		Ctx.AddSourceObject(OwnerChar);
		Ctx.AddInstigator(OwnerChar, OwnerChar);

		FGameplayEffectSpecHandle Spec =
			SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Ctx);
		if (Spec.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
		}
	}

	// VFX
	if (!AreaAttackVFX.IsNull())
	{
		if (UNiagaraSystem* LoadedVFX = AreaAttackVFX.LoadSynchronous())
		{
			OwnerChar = GetCharacterFromActorInfo(*CurrentActorInfo);
			if (OwnerChar)
			{
				FVector  SpawnLoc = OwnerChar->GetActorTransform().TransformPosition(VFXLocationOffset);
				FRotator SpawnRot = OwnerChar->GetActorRotation() + VFXRotationOffset;

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedVFX, SpawnLoc, SpawnRot, VFXScale);
			}
		}
	}
}

void ULRGA_EnemyAreaAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveMontage) return;

	if (CurrentActorInfo)
	{
		ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*CurrentActorInfo);
		if (OwnerChar && OwnerChar->GetMesh())
		{
			UnbindMontageCallbacks(OwnerChar->GetMesh()->GetAnimInstance());
		}
	}

	ActiveMontage = nullptr;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bInterrupted);
}

void ULRGA_EnemyAreaAttack::UnbindMontageCallbacks(UAnimInstance* AnimInstance)
{
	if (!AnimInstance) return;
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ULRGA_EnemyAreaAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(
		this, &ULRGA_EnemyAreaAttack::OnMontageNotifyBegin);
}