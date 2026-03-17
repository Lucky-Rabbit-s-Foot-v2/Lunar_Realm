// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LROutGameOverlayWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Common/LRCurrencyWidget.h"

void ULROutGameOverlayWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Message) Btn_Message->OnClicked.AddDynamic(this, &ULROutGameOverlayWidget::OnMessageButtonClicked);
	if (Btn_Setting) Btn_Setting->OnClicked.AddDynamic(this, &ULROutGameOverlayWidget::OnSettingButtonClicked);
}

void ULROutGameOverlayWidget::UnbindProperties()
{
	Btn_Message->OnClicked.Clear();
	Btn_Setting->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULROutGameOverlayWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Currency);
}

void ULROutGameOverlayWidget::OnMessageButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Message Button Clicked : Not implemented yet"));
}

void ULROutGameOverlayWidget::OnSettingButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::SETTING);
}
