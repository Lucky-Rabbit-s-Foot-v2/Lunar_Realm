// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRAetherWidget.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"


void ULRAetherWidget::BindToASC(UAbilitySystemComponent* ASC)
{
	LR_WARN(TEXT("1Binding Health Bar Widget to ASC"));

	if (!ASC) return;
	
	LR_WARN(TEXT("2Binding Health Bar Widget to ASC"));

	bool bFound = false;
	float CurrentAether = ASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetAetherAttribute(), bFound);
	UpdateAether(CurrentAether);
	LR_WARN(TEXT("3Binding Health Bar Widget to ASC"));

	ASC->GetGameplayAttributeValueChangeDelegate(ULRPlayerAttributeSet::GetAetherAttribute())
		.AddUObject(this, &ULRAetherWidget::OnAetherChanged);

	LR_WARN(TEXT("4Binding Health Bar Widget to ASC"));

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
	//UpdateAether(Data.NewValue);

	if (Data.NewValue > TargetAether)
	{
		if (Anim_GetAether)
		{
			PlayAnimation(Anim_GetAether);
		}
	}

	TargetAether = Data.NewValue;
}

void ULRAetherWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 숫자를 목표치까지 부드럽게 보간 (FInterpTo)
	if (!FMath::IsNearlyEqual(DisplayAether, TargetAether, 0.1f))
	{
		// 10.0f는 속도
		DisplayAether = FMath::FInterpTo(DisplayAether, TargetAether, InDeltaTime, 10.0f);

		if (Text_AetherAmount)
		{
			Text_AetherAmount->SetText(FText::AsNumber((int32)DisplayAether));
		}
	}
}
