// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRCoreAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Structures/Core/LRCore.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "System/LoggingSystem.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"


void ULRCoreAttributeSet::OnDamageExecuted(float InDamageDone, const FGameplayEffectModCallbackData& Data)
{
	if (InDamageDone <= 0.0f)
	{
		return;
	}

	ALRCore* Core = Cast<ALRCore>(GetOwningAbilitySystemComponent()->GetAvatarActor());

	if (!Core)
	{
		return;
	}

	if (Data.EffectSpec.DynamicAssetTags.HasTagExact(LRTags::Ability_Skill_InstantCoreDestroy))
	{
		// TEST
		LR_ERROR(TEXT("~~~~~~~~~~~~~ [Core] 보스 근접 공격 감지 - 즉시 파괴 ~~~~~~~~~~~~~"));
		Core->OnCoreDestroyed();
		return;
	}

	if (GetHealth() <= 0.0f)
	{
		Core->OnCoreDestroyed();
	}
}
