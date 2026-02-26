// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "System/LoggingSystem.h"

ULRPlayerAttributeSet::ULRPlayerAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitAether(0.0f);
	InitAttackPower(10.0f);
	InitDefense(0.0f);
}

void ULRPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == ULRAttributeSet::GetHealthAttribute())
	{
		if (NewValue < GetHealth())
		{
			if (ALRPlayerCharacter* PC = Cast<ALRPlayerCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
			{
				if (PC->IsInvincible())
				{
					NewValue = GetHealth();
					LR_INFO(TEXT("무적 상태 데미지 무시"));
				}
			}
		}
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void ULRPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	LR_WARN(TEXT("[AttributeSet] 데미지 들어옴! 변경된 속성: %s, 변경 후 수치: %f"),
		*Data.EvaluatedData.Attribute.GetName(), Data.EvaluatedData.Attribute.GetNumericValue(this));

	if (Data.EvaluatedData.Attribute == ULRAttributeSet::GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		LR_INFO(TEXT("[AttributeSet] 체력 Clamp 완료. 최종 체력: %f"), GetHealth());

		if (GetHealth() <= 0.0f)
		{
			LR_WARN(TEXT("Player Die - 체력이 0이 되었습니다."));
		}
	}
}
