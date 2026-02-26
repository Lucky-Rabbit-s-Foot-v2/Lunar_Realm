// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRHealthWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"

void ULRHealthWidget::BindToASC(UAbilitySystemComponent* ASC)
{
	LR_WARN(TEXT("Initializing Player Widget GAS with ASC: %s"), *GetNameSafe(ASC));

	if (!ASC)
	{
		return;
	}

	LR_WARN(TEXT("Initializing Player Widget GAS with ASC: %s"), *GetNameSafe(ASC));

	bool bFoundHealth = false;
	bool bFoundMaxHealth = false;

	CurrentHealth = ASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetHealthAttribute(), bFoundHealth);
	CurrentMaxHealth = ASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetMaxHealthAttribute(), bFoundMaxHealth);

	UpdateHealth(CurrentHealth, CurrentMaxHealth);

	ASC->GetGameplayAttributeValueChangeDelegate(ULRPlayerAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ULRHealthWidget::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(ULRPlayerAttributeSet::GetMaxHealthAttribute())
		.AddUObject(this, &ULRHealthWidget::OnMaxHealthChanged);
}

void ULRHealthWidget::UpdateHealth(float InCurrentHealth, float InMaxHealth)
{
	if (PBar_Health)
	{
		float Percent = (InMaxHealth > 0.0f) ? (InCurrentHealth / InMaxHealth) : 0.0f;
		PBar_Health->SetPercent(Percent);
	}
}

void ULRHealthWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	UpdateHealth(CurrentHealth, CurrentMaxHealth);
}

void ULRHealthWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	UpdateHealth(CurrentHealth, CurrentMaxHealth);
}
