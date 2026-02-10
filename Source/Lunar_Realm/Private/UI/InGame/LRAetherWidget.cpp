// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRAetherWidget.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"


void ULRAetherWidget::BindToASC(UAbilitySystemComponent* ASC)
{
	if (!ASC) return;

	bool bFound = false;
	float CurrentAether = ASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetAetherAttribute(), bFound);
	UpdateAether(CurrentAether);

	ASC->GetGameplayAttributeValueChangeDelegate(ULRPlayerAttributeSet::GetAetherAttribute())
		.AddUObject(this, &ULRAetherWidget::OnAetherChanged);
}

void ULRAetherWidget::UpdateAether(float Amount)
{
	if (Text_AetherAmount)
	{
		Text_AetherAmount->SetText(FText::AsNumber((int32)Amount));
	}
}

void ULRAetherWidget::OnAetherChanged(const FOnAttributeChangeData& Data)
{
	UpdateAether(Data.NewValue);
}
