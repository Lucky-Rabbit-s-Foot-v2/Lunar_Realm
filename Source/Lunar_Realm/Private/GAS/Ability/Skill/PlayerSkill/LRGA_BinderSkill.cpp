// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_BinderSkill.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_BinderSkill::ULRGA_BinderSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Binder;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_BINDER";
	SkillEffectID = "EFFECT_BINDER";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Binder.Cooldown")));

	StrikeDistance = 450.0f;
}
