// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_Iceball.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Subsystems/GameDataSubsystem.h"

ULRGA_Iceball::ULRGA_Iceball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_IceBall;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	AbilityTriggers.Add(TriggerData);
	
	SkillID = "SKILL_ICEBALL";
	SkillEffectID = "EFFECT_ICEBALL";
}

void ULRGA_Iceball::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	LR_INFO(TEXT("[ULRGA_Iceball] OnAbilityActivated 진입!"));

	if (!CachedInstigator || !ProjectileClass || !DamageEffectClass)
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
	InitData.StatusEffectClass = StatusEffectClass; // GE_Freeze
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
