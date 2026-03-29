// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_Debuff_Poison.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/GameDataSubsystem.h"
#include "System/LoggingSystem.h"

ULRGA_Debuff_Poison::ULRGA_Debuff_Poison()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = LRTags::Ability_Skill_Debuff_Poison;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
	
	//DT참조키
	SkillID = "SKILL_DEBUFF_POISON";
	SkillEffectID = "EFFECT_DEBUFF_POISON";
}

void ULRGA_Debuff_Poison::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    LR_INFO(TEXT("OnAbilityActivated 진입"));

    if (!CachedInstigator.IsValid())
    {
        LR_WARN(TEXT("CachedInstigator 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 에너미 기준 적대 팀 = 플레이어 팀 태그 반환
    FGameplayTag HostileTag = GetHostileTeamTag();
    if (!HostileTag.IsValid())
    {
        LR_WARN(TEXT("HostileTag 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // DataSubsystem에서 탐색 범위 읽기
    UGameInstance* GI = GetWorld()->GetGameInstance();
    check(GI);
    UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
    if (!DataSys)
    {
        LR_WARN(TEXT("[Poison] DataSubsystem 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FSkillEffectData& EffectData   = DataSys->GetSkillEffectData(SkillEffectID);

    // 가장 가까운 적대 액터 탐색 (베이스 헬퍼 사용)
    AActor* Target = FindNearestHostile(HostileTag, EffectData.Range);
    if (!Target)
    {
        LR_INFO(TEXT("범위 내 플레이어 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
    if (!TargetASC)
    {
        LR_WARN(TEXT("TargetASC 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 대상이 사망했으면 적용 안함.
    if (TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (!PoisonEffectClass)
    {
        LR_WARN(TEXT("PoisonEffectClass 미설정"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 에너미 ASC 기준으로 GE 스펙 생성 후 타겟 적용
    UAbilitySystemComponent* SourceASC = GetOwnerASC();
	if (!SourceASC)
	{
		LR_WARN(TEXT("SourceASC 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
    FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(PoisonEffectClass, 1.f, Context);

    if (SpecHandle.IsValid())
    {
        // 틱당 데미지(음수로 전달)
        SpecHandle.Data->SetByCallerTagMagnitudes.Add(LRTags::Data_Damage, -EffectData.Amount);
        SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}