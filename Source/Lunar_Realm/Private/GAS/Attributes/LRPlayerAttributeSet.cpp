// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "System/LoggingSystem.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"


void ULRPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 무적 판정
	if (Attribute == GetDamageAttribute())
	{
		if (ALRPlayerCharacter* PC = Cast<ALRPlayerCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
		{
			if (PC->IsInvincible())
			{
				NewValue = 0.0f;
				LR_INFO(TEXT("무적 상태"));
			}
		}
	}
}

FLinearColor ULRPlayerAttributeSet::GetDamageTextColor(float InDamage) const
{
	if (InDamage < 0.0f)
	{
		return FLinearColor::Green;
	}
	return FLinearColor::Red;
}

void ULRPlayerAttributeSet::OnDamageExecuted(float InDamageDone, const FGameplayEffectModCallbackData& Data)
{
	// 플러스 데미지 (공격)
	if (InDamageDone > 0.0f)
	{
		LR_INFO(TEXT("[Player] 데미지 %f 적중 최종 체력: %f"), InDamageDone, GetHealth());
		if (GetHealth() <= 0.0f)
		{
			LR_WARN(TEXT("플레이어 사망"));
		}
	}
	// 마이너스 데미지 (힐)
	else
	{
		LR_INFO(TEXT("[Player] 체력 %f 회복! 최종 체력: %f"), -InDamageDone, GetHealth());
	}
}
