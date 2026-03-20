// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LROptionNameButtonsWidget.h"

#include "Engine/GameInstance.h"

#include "Components/TextBlock.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"

void ULROptionNameButtonsWidget::NativePreConstruct()
{
	Txt_Name->SetText(OptionName);
}

void ULROptionNameButtonsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OnOptionButtonClickedDel.AddUniqueDynamic(OptionManager, &UOptionManagerSubsystem::UpdateOptionValue);
	}
}

void ULROptionNameButtonsWidget::NativeDestruct()
{
	OnOptionButtonClickedDel.Clear();

	Super::NativeDestruct();
}

void ULROptionNameButtonsWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Low->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULROptionNameButtonsWidget::OnLowButtonClicked);
	Btn_Medium->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULROptionNameButtonsWidget::OnMediumButtonClicked);
	Btn_High->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULROptionNameButtonsWidget::OnHighButtonClicked);
}

void ULROptionNameButtonsWidget::UnbindProperties()
{
	Btn_High->OnLRButtonClickedDel.Clear();
	Btn_Medium->OnLRButtonClickedDel.Clear();
	Btn_Low->OnLRButtonClickedDel.Clear();
	
	Super::UnbindProperties();
}

void ULROptionNameButtonsWidget::OnLowButtonClicked()
{
	CurrentNumber = 0;
	OnOptionButtonClickedDel.Broadcast(SettingType, CurrentNumber);
}

void ULROptionNameButtonsWidget::OnMediumButtonClicked()
{
	CurrentNumber = 1;
	OnOptionButtonClickedDel.Broadcast(SettingType, CurrentNumber);
}

void ULROptionNameButtonsWidget::OnHighButtonClicked()
{
	CurrentNumber = 2;
	OnOptionButtonClickedDel.Broadcast(SettingType, CurrentNumber);
}