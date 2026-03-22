// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_BloodPool.h"
#include "Structures/SkillActor/LRAoEActor.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Units/LRCharacter.h"
#include "Subsystems/GameDataSubsystem.h"
#include "System/LoggingSystem.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_BloodPool::ULRGA_BloodPool()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_BloodPool;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_BLOODPOOL";
	SkillEffectID = "EFFECT_BLOODPOOL";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.BloodPool.Cooldown")));
}

void ULRGA_BloodPool::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	LR_INFO(TEXT("[BloodPool/Anubis] 스킬 발동 시작!"));
		
	if (!CachedInstigator)
	{
		LR_WARN(TEXT("에러: CachedInstigator가 없습니다!"));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	if (!AoEActorClass)
	{
		LR_WARN(TEXT("에러: 블루프린트에 AoEActorClass가 안 들어있습니다!"));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	FGameplayEventData DummyPayload;
	OnHitEventReceived(DummyPayload);

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}

void ULRGA_BloodPool::OnHitEventReceived(FGameplayEventData InPayload)
{
	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys) return;

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	FVector SpawnLocation = CachedInstigator->GetActorLocation();

	SpawnLocation.Z -= 88.0f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = const_cast<ALRCharacter*>(CachedInstigator.Get());
	SpawnParams.Instigator = const_cast<ALRCharacter*>(CachedInstigator.Get());

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALRAoEActor* SpawnedAoE = GetWorld()->SpawnActor<ALRAoEActor>(AoEActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedAoE)
	{
		LR_INFO(TEXT("[BloodPool/Anubis] 장판 스폰 완벽 성공!"));
		float Radius = EffectData.Range > 0.f ? EffectData.Range : 400.0f;
		float Duration = EffectData.Lifetime > 0.f ? EffectData.Lifetime : 5.0f;

		SpawnedAoE->InitAoE(
			GetOwnerASC(),
			DamageEffectClass,
			GetHostileTeamTag(),
			SkillData.ResourceID,
			Radius,
			Duration
		);
	}
	else
	{
		LR_WARN(TEXT("에러: 장판 스폰에 실패했습니다! (충돌 문제일 가능성 높음)"));
	}
}