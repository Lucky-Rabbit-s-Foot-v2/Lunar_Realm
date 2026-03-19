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

	if (GetHealth() <= 0.0f)
	{
		Core->OnCoreDestroyed();
	}
}
