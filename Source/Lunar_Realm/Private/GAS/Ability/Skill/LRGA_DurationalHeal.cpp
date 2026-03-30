// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_DurationalHeal.h"
#include "Lunar_Realm.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"

ULRGA_DurationalHeal::ULRGA_DurationalHeal()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_DurationHeal;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
	
	//DT 참조키
	SkillEffectID = "EFFECT_DURATION_HEAL";
	SkillID = "SKILL_DURATION_HEAL";
}

void ULRGA_DurationalHeal::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	LR_INFO(TEXT("OnAbilityActivated 진입"));

	if (!CachedInstigator.IsValid())
	{
		LR_WARN(TEXT("CachedInstigator 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const AActor* HealTarget = CachedInstigator.IsValid() ? CachedTarget.Get() : Cast<AActor>(CachedInstigator.Get());

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(HealTarget));
	
	if (!TargetASC)
	{
		LR_WARN(TEXT("TargetASC 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
	{
		LR_WARN(TEXT("타겟 사망 상태 — 힐 취소"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (!HealEffectClass)
	{
		LR_WARN(TEXT("HealEffectClass 미설정"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UGameInstance* GI = GetWorld()->GetGameInstance();
	check(GI);
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		LR_WARN(TEXT("[DurationHeal] DataSubsystem 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);

	// Self에게 Duration Heal GE 적용
	FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(HealEffectClass, 1.f, Context);

	if (SpecHandle.IsValid())
	{
		// 틱당 회복량 — GE_Heal_Duration의 Period와 곱해서 총 회복량 계산됨
		SpecHandle.Data->SetByCallerTagMagnitudes.Add(LRTags::Data_Heal, EffectData.Amount);
		TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
