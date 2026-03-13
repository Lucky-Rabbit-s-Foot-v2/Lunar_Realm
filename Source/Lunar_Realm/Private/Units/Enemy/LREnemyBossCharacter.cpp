// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyBossCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"

ALREnemyBossCharacter::ALREnemyBossCharacter()
{
}

void ALREnemyBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("[Boss] ASC를 찾을 수 없음"));
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(
		ULREnemyAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ALREnemyBossCharacter::OnBossHealthChanged);
}

void ALREnemyBossCharacter::OnBossHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("[Boss] ASC를 찾을 수 없음"));
		return;
	}

	const float MaxHealth = ASC->GetNumericAttribute(ULREnemyAttributeSet::GetMaxHealthAttribute());
	if (MaxHealth <= 0.0f)
	{
		return;
	}

	const float HealthPercent = Data.NewValue / MaxHealth;
	const int32 NewPhase = CalculatePhase(HealthPercent);

	if (NewPhase != CurrentPhase)
	{
		CurrentPhase = NewPhase;
		// TEST
		LR_ERROR(TEXT("[Boss] %d번째 페이즈로 전환"), CurrentPhase);

		if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICtrl->GetBlackboardComponent())
			{
				BB->SetValueAsInt(LRBBKeys::CurrentPhase, CurrentPhase);
			}
			else
			{
				LR_ERROR(TEXT("[Boss] BB 찾을 수 없음"));
			}
		}
		else
		{
			LR_ERROR(TEXT("[Boss] 컨트롤러 찾을 수 없음"));
		}
	}
}

int32 ALREnemyBossCharacter::CalculatePhase(float HealthPercent) const
{
	for (int32 i = 0; i < PhaseThresholds.Num(); ++i)
	{
		if (HealthPercent > PhaseThresholds[i])
		{
			return i;
		}
	}
	return PhaseThresholds.Num();
}
