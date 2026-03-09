// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRCoreAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Structures/Core/LRCore.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "System/LoggingSystem.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"

ULRCoreAttributeSet::ULRCoreAttributeSet()
{
	InitHealth(1000.0f);
	InitMaxHealth(10000.0f);
}

void ULRCoreAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);


	if (Attribute == ULRAttributeSet::GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == ULRAttributeSet::GetDamageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

}

void ULRCoreAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == ULRAttributeSet::GetDamageAttribute())
	{
		float LocalDamageDone = GetDamage();

		SetDamage(0.0f);

		if (LocalDamageDone > 0.0f)
		{
			float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			if (ALRCore* Core = Cast<ALRCore>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
			{
				if (UGameInstance* GI = Core->GetGameInstance())
				{
					if (UUIManagerSubsystem* UIManager = GI->GetSubsystem<UUIManagerSubsystem>())
					{
						FVector HitLocation = Core->GetActorLocation() + FVector(0.f, 0.f, 0.f);
						FLinearColor DamageColor = FLinearColor::White;
						UIManager->ShowDamageText(LocalDamageDone, HitLocation, DamageColor);
					}
				}
				if (GetHealth() <= 0.0f)
				{
					Core->OnCoreDestroyed();
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == ULRAttributeSet::GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		LR_INFO(TEXT("[AttributeSet] 체력 Clamp 완료. 최종 체력: %f"), GetHealth());
	}
}
