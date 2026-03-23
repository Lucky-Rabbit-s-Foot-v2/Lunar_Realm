// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LROptionNameBarWidget.h"

#include "Engine/GameInstance.h"

#include "Components/TextBlock.h"
#include "Components/Slider.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"

void ULROptionNameBarWidget::NativePreConstruct()
{
	Txt_Name->SetText(OptionName);
}

void ULROptionNameBarWidget::BindProperties()
{
	Super::BindProperties();

	Slider_Option->OnValueChanged.AddDynamic(this, &ULROptionNameBarWidget::OnSliderValueChanged);

	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OnOptionBarChangedDel.AddUniqueDynamic(OptionManager, &UOptionManagerSubsystem::UpdateOptionValue);
	}
}

void ULROptionNameBarWidget::UnbindProperties()
{
	Slider_Option->OnValueChanged.Clear();
	OnOptionBarChangedDel.Clear();

	Super::UnbindProperties();
}

void ULROptionNameBarWidget::OnSliderValueChanged(float Value)
{
	// 슬라이더의 0.0 ~ 1.0 값을 정수 범위(Min~Max)로 변환
	int32 Range = MaxValue - MinValue;
	CurrentValue = FMath::RoundToInt(MinValue + (Value * Range));

	// 옵션 매니저에 알림
	if (OnOptionBarChangedDel.IsBound())
	{
		OnOptionBarChangedDel.Broadcast(SettingType, CurrentValue);
	}
}