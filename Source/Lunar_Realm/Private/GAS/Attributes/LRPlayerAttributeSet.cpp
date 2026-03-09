// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "System/LoggingSystem.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"

ULRPlayerAttributeSet::ULRPlayerAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitAether(0.0f);
	InitAttackPower(4.0f);
	InitDefense(0.0f);
}

void ULRPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);


	if (Attribute == ULRAttributeSet::GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == ULRAttributeSet::GetDamageAttribute())
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

void ULRPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

			LR_WARN(TEXT("[AttributeSet] 데미지 %f 적중! 최종 체력: %f"), LocalDamageDone, GetHealth());

			if (ALRPlayerCharacter* PC = Cast<ALRPlayerCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
			{
				if (UGameInstance* GI = PC->GetGameInstance())
				{
					if (UUIManagerSubsystem* UIManager = GI->GetSubsystem<UUIManagerSubsystem>())
					{
						FVector HitLocation = PC->GetActorLocation() + FVector(0.f, 0.f, 100.f);
						UIManager->ShowDamageText(LocalDamageDone, HitLocation);
					}
				}
				if (GetHealth() <= 0.0f)
				{
					LR_WARN(TEXT("플레이어 사망"));
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
