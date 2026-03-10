// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_InstantHeal.h"
#include "Lunar_Realm.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"


ULRGA_InstantHeal::ULRGA_InstantHeal()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = LRTags::Ability_Skill_InstantHeal;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
	
	// DT참조키
	SkillID = "SKILL_INSTANT_HEAL";
	SkillEffectID = "EFFECT_INSTANT_HEAL";
}

void ULRGA_InstantHeal::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    LR_INFO(TEXT("OnAbilityActivated 진입"));

    // Heal은 Self 대상이므로 CachedInstigator 필수
    if (!CachedInstigator)
    {
        LR_WARN(TEXT("CachedInstigator 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 사망 상태 체크 — 죽은 캐릭터에게 힐 적용 방지
	const AActor* HealTarget = CachedTarget ? CachedTarget.Get() : Cast<AActor>(CachedInstigator.Get());

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(HealTarget));
	if (!TargetASC)
	{
		LR_WARN(TEXT("TargetASC 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 타겟 사망 상태 체크
	if (TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
	{
		LR_WARN(TEXT("타겟 사망 상태 — 힐 취소"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

    // DataSubsystem에서 회복량 읽기
    UGameInstance* GI = GetWorld()->GetGameInstance();
    check(GI);
    UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
    if (!DataSys)
    {
        LR_WARN(TEXT("DataSubsystem 없음"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

	if (!HealEffectClass)
	{
		LR_WARN(TEXT("HealEffectClass 미설정"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
    const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);

    // TargetASC에 Instant Heal GE 적용
    FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HealEffectClass, 1.f, Context);

    if (SpecHandle.IsValid())
    {
        SpecHandle.Data->SetByCallerTagMagnitudes.Add(LRTags::Data_Heal, EffectData.Amount);
        TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}