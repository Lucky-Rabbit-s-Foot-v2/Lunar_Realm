// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_DemoniaSkill.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_DemoniaSkill::ULRGA_DemoniaSkill()
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Demonia;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_DEMONIA";
	SkillEffectID = "EFFECT_DEMONIA";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Demonia.Cooldown")));
}
