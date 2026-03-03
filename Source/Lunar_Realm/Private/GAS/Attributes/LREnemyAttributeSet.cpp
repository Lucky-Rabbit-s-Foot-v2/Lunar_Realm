// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "System/LoggingSystem.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"

ULREnemyAttributeSet::ULREnemyAttributeSet()
{
}

void ULREnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	if (Attribute == GetAttackAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxAttack);
	}

	if (Attribute == GetSpeedAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxSpeed);
	}
	if(Attribute == GetDamageAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxAttack);
	}
}

void ULREnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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
			if (ALREnemyCharacter* EC = Cast<ALREnemyCharacter>(GetOwningAbilitySystemComponent()->GetAvatarActor()))
			{
				if (UGameInstance* GI = EC->GetGameInstance())
				{
					if (UUIManagerSubsystem* UIManager = GI->GetSubsystem<UUIManagerSubsystem>())
					{
						FVector HitLocation = EC->GetActorLocation() + FVector(0.f, 0.f, 100.f);
						UIManager->ShowDamageText(LocalDamageDone, HitLocation);
					}
				}
				if (GetHealth() <= 0.0f)
				{
					EC->OnDie();
				}
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == ULRAttributeSet::GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		LR_INFO(TEXT("[AttributeSet] 체력 Clamp 완료. 최종 체력: %f"), GetHealth());
	}

	//if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	//{
	//	SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

	//	if (GetHealth() <= 0.0f)
	//	{
	//		UAbilitySystemComponent* ASC = Data.Target.AbilityActorInfo.Get()->AbilitySystemComponent.Get();
	//		if (!ASC)
	//		{
	//			return;
	//		}

	//		AActor* OwnerActor = ASC->GetOwnerActor();

	//		if (ALREnemyCharacter* EnemyChar = Cast<ALREnemyCharacter>(OwnerActor))
	//		{
	//			EnemyChar->OnDie();
	//		}
	//	}
	//}
}
