// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LROutGamePersistentWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Common/LRCurrencyWidget.h"

void ULROutGamePersistentWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Message) Btn_Message->OnClicked.AddDynamic(this, &ULROutGamePersistentWidget::OnMessageButtonClicked);
	if (Btn_Setting) Btn_Setting->OnClicked.AddDynamic(this, &ULROutGamePersistentWidget::OnSettingButtonClicked);
}

void ULROutGamePersistentWidget::UnbindProperties()
{
	Btn_Message->OnClicked.Clear();
	Btn_Setting->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULROutGamePersistentWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Currency);
}

void ULROutGamePersistentWidget::OnMessageButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Message Button Clicked : Not implemented yet"));
}

void ULROutGamePersistentWidget::OnSettingButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::SETTING);
}
