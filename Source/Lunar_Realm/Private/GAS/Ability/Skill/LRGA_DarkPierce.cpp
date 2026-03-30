// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_DarkPierce.h"

#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Subsystems/GameDataSubsystem.h"

ULRGA_DarkPierce::ULRGA_DarkPierce()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Pierce;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	AbilityTriggers.Add(TriggerData);
	
	
	//DT참조키
	SkillID = "SKILL_PIERCE";
	SkillEffectID = "EFFECT_PIERCE";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Pierce.Cooldown")));
}

void ULRGA_DarkPierce::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	LR_INFO(TEXT("[ULRGA_Pierce] OnAbilityActivated 진입!"));

	if (!CachedInstigator.IsValid() || !ProjectileClass || !DamageEffectClass)
	{
		LR_WARN(TEXT("필수 데이터 누락"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UGameInstance* GI = GetWorld()->GetGameInstance();
	check(GI);
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		LR_WARN(TEXT("말도 안돼 데이터시스템이 없다니!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillSpawnData& SpawnData = DataSys->GetSkillSpawnData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	FSkillObjectInitData InitData;
	InitData.DamageEffectClass = DamageEffectClass;
	InitData.StatusEffectClass = nullptr;  // Pierce는 상태이상 없음
	InitData.InstigatorASC     = GetOwnerASC();
	InitData.ResourceID        = SkillData.ResourceID;
	InitData.SkillEffectID     = SkillEffectID;
	InitData.Damage            = EffectData.Amount;
	InitData.Speed             = EffectData.Speed;
	InitData.Lifetime          = EffectData.Lifetime;
	InitData.SpawnData         = SpawnData;

	SpawnProjectiles(ProjectileClass, InitData);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
