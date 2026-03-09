// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRNameButtonsWidget.h"

#include "Engine/GameInstance.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"

void ULRNameButtonsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OnButtonClickedDel.AddUniqueDynamic(OptionManager, &UOptionManagerSubsystem::UpdateOptionValue);
	}
	
}

void ULRNameButtonsWidget::NativeDestruct()
{
	OnButtonClickedDel.Clear();

	Super::NativeDestruct();
}

void ULRNameButtonsWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Low) Btn_Low->OnClicked.AddDynamic(this, &ULRNameButtonsWidget::OnLowButtonClicked);
	if (Btn_Medium) Btn_Medium->OnClicked.AddDynamic(this, &ULRNameButtonsWidget::OnMediumButtonClicked);
	if (Btn_High) Btn_High->OnClicked.AddDynamic(this, &ULRNameButtonsWidget::OnHighButtonClicked);
}

void ULRNameButtonsWidget::UnbindProperties()
{
	Btn_High->OnClicked.Clear();
	Btn_Medium->OnClicked.Clear();
	Btn_Low->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRNameButtonsWidget::SetName(FText Name)
{
	if(Txt_Name)
	{
		Txt_Name->SetText(Name);
	}
}

void ULRNameButtonsWidget::OnLowButtonClicked()
{
	CurrentNumber = 0;
	OnButtonClickedDel.Broadcast(SettingType, CurrentNumber);
}

void ULRNameButtonsWidget::OnMediumButtonClicked()
{
	CurrentNumber = 1;
	OnButtonClickedDel.Broadcast(SettingType, CurrentNumber);
}

void ULRNameButtonsWidget::OnHighButtonClicked()
{
	CurrentNumber = 2;
	OnButtonClickedDel.Broadcast(SettingType, CurrentNumber);
}
