// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Enemy/LRGA_BossInstantAttack.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_BossInstantAttack::ULRGA_BossInstantAttack()
{
	AbilityTriggers.Reset();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_BossInstantAttack;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.RemoveTag(LRTags::Ability_Skill_InstantAttack);
	TempTags.AddTag(LRTags::Ability_Skill_BossInstantAttack);
	SetAssetTags(TempTags);

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(LRTags::Ability_Skill_BossInstantAttack_Cooldown);
}

void ULRGA_BossInstantAttack::ModifyDamageSpec(FGameplayEffectSpecHandle& InSpecHandle)
{
	if (InSpecHandle.IsValid())
	{
		InSpecHandle.Data->DynamicAssetTags.AddTag(LRTags::Ability_Skill_InstantCoreDestroy);
	}
}
