// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_Heal.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"


ULRGA_Heal::ULRGA_Heal()
{


	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Heal;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	FGameplayTagContainer TempTags;
	TempTags.AddTag(LRTags::Ability_Skill_Heal);
	SetAssetTags(TempTags);
	//AbilityTags = TempTags;

	ActivationBlockedTags.AddTag(LRTags::State_Dead);


	//InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//
	//FGameplayTagContainer TempTags = GetAssetTags();
	//TempTags.AddTag(LRTags::Ability_Skill_Heal);
	//SetAssetTags(TempTags);
	//
	////(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	//FAbilityTriggerData TriggerData;
	//TriggerData.TriggerTag = LRTags::Ability_Skill_Heal;
	//TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	//AbilityTriggers.Add(TriggerData);

	//SkillID = "SKILL_PLAYERHEAL";
	//SkillEffectID = "EFFECT_PLAYERHEAL";

	//CooldownTagContainer.Reset();
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.Heal")));





}

//void ULRGA_Heal::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
//	const FGameplayAbilityActivationInfo ActivationInfo)
//{
//	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
//	{
//		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
//		return;
//	}
//
//	if (HealEffectClass && ActorInfo->AbilitySystemComponent.IsValid())
//	{
//		FGameplayEffectContextHandle ContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
//		FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(HealEffectClass, 1.0f, ContextHandle);
//
//		if (SpecHandle.IsValid())
//		{
//			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
//			UE_LOG(LogTemp, Warning, TEXT("체력 회복 성공!"));
//		}
//	}
//
//	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
//}

void ULRGA_Heal::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	if (!CommitAbility(InHandle, InActorInfo, InActivationInfo))
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	if (HealEffectClass && InActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayEffectContextHandle ContextHandle = InActorInfo->AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = InActorInfo->AbilitySystemComponent->MakeOutgoingSpec(HealEffectClass, 1.0f, ContextHandle);

		if (SpecHandle.IsValid())
		{
			InActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("체력 회복 성공!"));
		}
	}

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}

bool ULRGA_Heal::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return false;

	float CurrentHealth = ASC->GetNumericAttribute(ULRPlayerAttributeSet::GetHealthAttribute());
	float MaxHealth = ASC->GetNumericAttribute(ULRPlayerAttributeSet::GetMaxHealthAttribute());

	if (CurrentHealth >= MaxHealth)
	{
		UE_LOG(LogTemp, Warning, TEXT("체력이 이미 가득 참"));
		return false;
	}

	return true;
}

//const FGameplayTagContainer* ULRGA_Heal::GetCooldownTags() const
//{
//	static FGameplayTagContainer CooldownTags;
//
//	if (CooldownTags.IsEmpty())
//	{
//		CooldownTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.Heal")));
//	}
//
//	return &CooldownTags;
//}
//
//void ULRGA_Heal::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
//{
//	UGameplayEffect* CostGE = GetCostGameplayEffect();
//	if (!CostGE)
//	{
//		return;
//	}
//
//	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostGameplayEffectClass);
//
//	if (UGameDataSubsystem* DataSys = GetWorld()->GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
//	{
//		const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(FName(SkillEffectID));
//
//		if (EffectData.Cost > 0.f)
//		{
//			SpecHandle.Data->SetByCallerTagMagnitudes.Add(LRTags::Data_Cost, EffectData.Cost);
//		}
//	}
//
//
//	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
//}
