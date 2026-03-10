// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_Fireball.h"

#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Subsystems/GameDataSubsystem.h"

ULRGA_Fireball::ULRGA_Fireball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//이벤트 태그로 동작하도록 Trigger등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Fireball;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	
	AbilityTriggers.Add(TriggerData);

	
	//DT 참조 키
	SkillID = "SKILL_FIREBALL";
	SkillEffectID = "EFFECT_FIREBALL";
}

void ULRGA_Fireball::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	LR_INFO(TEXT("[ULRGA_Fireball] OnAbilityActivated 진입!"));
	
	// 유효성 검사
	if (!CachedInstigator || !ProjectileClass || !DamageEffectClass)
	{
		LR_WARN(TEXT("필수 데이터 누락"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// DT에서 스킬 데이터 읽기
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
	
	//FSkillObjectInitData 채우기
	FSkillObjectInitData InitData;
	InitData.DamageEffectClass = DamageEffectClass;
	InitData.StatusEffectClass = StatusEffectClass;  // GE_BURN
	InitData.InstigatorASC     = GetOwnerASC();
	InitData.ResourceID		   = SkillData.ResourceID;
	InitData.SkillEffectID	   = SkillEffectID;
	InitData.Damage            = EffectData.Amount;
	InitData.Speed             = EffectData.Speed;
	InitData.Lifetime          = EffectData.Lifetime;
	InitData.SpawnData         = SpawnData;	
	
	// 투사체 Spawn
	SpawnProjectiles(ProjectileClass, InitData);

	// EndAbility 
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
