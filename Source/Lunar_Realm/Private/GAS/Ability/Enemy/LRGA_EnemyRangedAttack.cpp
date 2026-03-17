#include "GAS/Ability/Enemy/LRGA_EnemyRangedAttack.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Subsystems/GameDataSubsystem.h"
#include "System/LoggingSystem.h"
#include "Units/LRCharacter.h"
	
const FName ULRGA_EnemyRangedAttack::ShootNotifyName = TEXT("BasicShoot");

ULRGA_EnemyRangedAttack::ULRGA_EnemyRangedAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Skill_EnemyRangedShoot);
	SetAssetTags(TempTags);

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_EnemyRangedShoot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_ENEMY_RANGED_SHOOT";
	SkillEffectID = "EFFECT_ENEMY_RANGED_SHOOT";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.EnemyRangedShoot.Cooldown")));
}

void ULRGA_EnemyRangedAttack::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	bProjectileSpawned = false;
	ActiveMontage = nullptr;

	if (!ProjectileClass || !DamageEffectClass)
	{
		LR_WARN(TEXT("[EnemyRangedAttack] ProjectileClass 또는 DamageEffectClass 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* MontageToPlay =
		Cast<UAnimMontage>(const_cast<UObject*>(CachedOptionalObject.Get()));
	if (!MontageToPlay)
	{
		LR_WARN(TEXT("[EnemyRangedAttack] 몽타주 없음 — 즉시 투사체 스폰"));
		// 몽타주 없으면 즉시 스폰
		UGameInstance* GI = GetWorld()->GetGameInstance();
		UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
		if (DataSys)
		{
			const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
			const FSkillSpawnData& SpawnData = DataSys->GetSkillSpawnData(SkillEffectID);
			const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

			FSkillObjectInitData InitData;
			InitData.DamageEffectClass = DamageEffectClass;
			InitData.InstigatorASC = GetOwnerASC();
			InitData.ResourceID = SkillData.ResourceID;
			InitData.SkillEffectID = SkillEffectID;
			InitData.Damage = EffectData.Amount;
			InitData.Speed = EffectData.Speed;
			InitData.Lifetime = EffectData.Lifetime;
			InitData.SpawnData = SpawnData;

			SpawnProjectiles(ProjectileClass, InitData);
		}
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*ActorInfo);
	UAnimInstance* AnimInstance =
		(OwnerChar && OwnerChar->GetMesh()) ? OwnerChar->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		LR_WARN(TEXT("[EnemyRangedAttack] AnimInstance 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	float MontageLength = AnimInstance->Montage_Play(MontageToPlay, 1.f);
	if (MontageLength <= 0.f)
	{
		LR_WARN(TEXT("[EnemyRangedAttack] 몽타주 재생 실패"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ActiveMontage = MontageToPlay;

	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ULRGA_EnemyRangedAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(
		this, &ULRGA_EnemyRangedAttack::OnMontageNotifyBegin);
	AnimInstance->OnMontageEnded.AddDynamic(this, &ULRGA_EnemyRangedAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
		this, &ULRGA_EnemyRangedAttack::OnMontageNotifyBegin);
}

void ULRGA_EnemyRangedAttack::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
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

	FAnimMontageInstance* MontageInstance = AnimInstance->GetMontageInstanceForID(Payload.MontageInstanceID);
	if (!MontageInstance || MontageInstance->Montage != ActiveMontage || NotifyName != ShootNotifyName)
	{
		LR_WARN(TEXT("[%s] : MontageInstance Or ActiveMontage MisMatch Or Not ShootNotifyName"), *GetName());
		return;
	}
	if (bProjectileSpawned)
	{
		return;
	}
	bProjectileSpawned = true;

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys)
	{
		LR_WARN(TEXT("[EnemyRangedAttack] DataSubsystem 없음"));
		return;
	}

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillSpawnData& SpawnData = DataSys->GetSkillSpawnData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	FSkillObjectInitData InitData;
	InitData.DamageEffectClass = DamageEffectClass;
	InitData.InstigatorASC = GetOwnerASC();
	InitData.ResourceID = SkillData.ResourceID;
	InitData.SkillEffectID = SkillEffectID;
	InitData.Damage = EffectData.Amount;
	InitData.Speed = EffectData.Speed;
	InitData.Lifetime = EffectData.Lifetime;
	InitData.SpawnData = SpawnData;

	SpawnProjectiles(ProjectileClass, InitData);
}

void ULRGA_EnemyRangedAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActiveMontage)
	{
		LR_WARN(TEXT("[EnemyRangedAttack] OnMontageEnded — Montage 불일치, 무시"));
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

void ULRGA_EnemyRangedAttack::UnbindMontageCallbacks(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
	{
		return;
	}
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ULRGA_EnemyRangedAttack::OnMontageEnded);
	AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(
		this, &ULRGA_EnemyRangedAttack::OnMontageNotifyBegin);
}