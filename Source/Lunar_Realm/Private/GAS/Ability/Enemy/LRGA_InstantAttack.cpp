#include "GAS/Ability/Enemy/LRGA_InstantAttack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "System/LoggingSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Units/LRAIController.h"
#include "Units/LRCharacter.h"

const FName ULRGA_InstantAttack::HitNotifyName = TEXT("BasicHit");

ULRGA_InstantAttack::ULRGA_InstantAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Skill_InstantAttack);
	SetAssetTags(TempTags);
	
	//(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_InstantAttack;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_INSTANT_ATTACK";
	SkillEffectID = "EFFECT_INSTANT_ATTACK";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.InstantAttack.Cooldown")));
}

void ULRGA_InstantAttack::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bDamageApplied = false;
	ActiveMontage = nullptr;

	const AActor* TargetActor = Cast<const AActor>(CachedTarget);
	if (!TargetActor)
	{
		LR_WARN(TEXT("[InstantAttack] 타겟 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(TargetActor));
	if (!TargetASC || !DamageEffectClass)
	{
		LR_WARN(TEXT("[InstantAttack] TargetASC 또는 DamageEffectClass 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// EventData에서 전달된 몽타주 꺼내기
	UAnimMontage* MontageToPlay =
		Cast<UAnimMontage>(const_cast<UObject*>(CachedOptionalObject.Get()));
	if (!MontageToPlay)
	{
		LR_WARN(TEXT("[InstantAttack] 몽타주 없음 — 즉시 데미지 적용"));
		UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
		if (SourceASC)
		{
			FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
			Ctx.AddSourceObject(ActorInfo->AvatarActor.Get());
			Ctx.AddInstigator(ActorInfo->AvatarActor.Get(), ActorInfo->AvatarActor.Get());
			FGameplayEffectSpecHandle Spec =
				SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Ctx);
			if (Spec.IsValid())
			{
				SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
			}
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*ActorInfo);
	UAnimInstance* AnimInstance =
		(OwnerChar && OwnerChar->GetMesh()) ? OwnerChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		LR_WARN(TEXT("[InstantAttack] AnimInstance 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	float MontageLength = AnimInstance->Montage_Play(MontageToPlay, 1.f);
	if (MontageLength <= 0.f)
	{
		LR_WARN(TEXT("[InstantAttack] 몽타주 재생 실패"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ActiveMontage = MontageToPlay;

	// 풀 재사용 대비: 먼저 Remove 후 Add
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ULRGA_InstantAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ULRGA_InstantAttack::OnMontageNotifyBegin);
	AnimInstance->OnMontageEnded.AddDynamic(this, &ULRGA_InstantAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ULRGA_InstantAttack::OnMontageNotifyBegin);

	// EndAbility 호출 안 함 -> BT Task InProgress 유지
}

void ULRGA_InstantAttack::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	// AnimInstance 가져오기
	ALRCharacter* OwnerChar = CurrentActorInfo ? GetCharacterFromActorInfo(*CurrentActorInfo) : nullptr;
	UAnimInstance* AnimInstance =
		(OwnerChar && OwnerChar->GetMesh()) ? OwnerChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		LR_WARN(TEXT("[%s] : AnimInstance 없음!"), *GetName());
		return;
	}

	// 우리가 재생한 몽타주의 "BasicHit" 노티파이인지 확인
	FAnimMontageInstance* MontageInstance = AnimInstance->GetMontageInstanceForID(Payload.MontageInstanceID);
	if (!MontageInstance || MontageInstance->Montage != ActiveMontage || NotifyName != HitNotifyName)
	{
		LR_WARN(TEXT("[%s] : MontageInstance Or ActiveMontage MisMatch Or Not HitNotifyName"), *GetName());
		return;
	}
	if (bDamageApplied)
	{
		return; // 중복 발동 방지
	}
	bDamageApplied = true;

	const AActor* TargetActor = Cast<const AActor>(CachedTarget);
	if (!TargetActor || !CurrentActorInfo)
	{
		LR_WARN(TEXT("[InstantAttack] OnNotify: CachedTarget cast 실패 또는 CurrentActorInfo 없음"));
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(TargetActor));
	UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();

	if (!TargetASC || !SourceASC || !DamageEffectClass)
	{
		LR_WARN(TEXT("[InstantAttack] OnNotify: TargetASC=%d, SourceASC=%d, DamageEffect=%d"),
			TargetASC != nullptr, SourceASC != nullptr, DamageEffectClass != nullptr);
		return;
	}

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(CurrentActorInfo->AvatarActor.Get());
	Ctx.AddInstigator(CurrentActorInfo->AvatarActor.Get(), CurrentActorInfo->AvatarActor.Get());

	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Ctx);
	if (Spec.IsValid())
	{
		SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
	}

	// VFX 재생
	if (!AttackVFX.IsNull())
	{
		if (UNiagaraSystem* LoadedVFX = AttackVFX.LoadSynchronous())
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

void ULRGA_InstantAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveMontage)
	{
		return;
	}

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

void ULRGA_InstantAttack::UnbindMontageCallbacks(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
	{
		return;
	}
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ULRGA_InstantAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(
		this, &ULRGA_InstantAttack::OnMontageNotifyBegin);
}
