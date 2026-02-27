// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_Arrow.h"

#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"

ULRGA_Arrow::ULRGA_Arrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//이벤트 태그로 발동되도록 Trigger 등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Arrow;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	
	AbilityTriggers.Add(TriggerData);
}

void ULRGA_Arrow::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	LR_INFO(TEXT("[LRGA_Arrow] OnAbilityActivated 진입!"));
	
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
    InitData.StatusEffectClass = nullptr;  // Arrow는 상태이상 없음
    InitData.InstigatorASC     = GetOwnerASC();
	InitData.ResourceID		   = SkillData.ResourceID;
	InitData.SkillEffectID	   = SkillEffectID;
    InitData.Damage            = EffectData.Damage;
    InitData.Speed             = EffectData.Speed;
    InitData.Lifetime          = EffectData.Lifetime;
	InitData.SpawnData         = SpawnData;	

    // 투사체 Spawn
	SpawnProjectiles(ProjectileClass, InitData);

    // EndAbility 
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
