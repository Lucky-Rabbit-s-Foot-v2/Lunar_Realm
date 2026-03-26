// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LRSettingPopupWidget.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Setting/LRSettingScrollWidget.h"
#include "UI/Core/LRButtonWidget.h"

void ULRSettingPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRSettingPopupWidget::NativeDestruct()
{
	OnCloseUIRequestedDel.Clear();

	Super::NativeDestruct();
}

void ULRSettingPopupWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(SettingScrollWidget);
}

void ULRSettingPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Close) Btn_Close->OnLRButtonClickedDel.AddDynamic(this, &ULRSettingPopupWidget::OnCloseButtonClicked);
	if (Btn_Save) Btn_Save->OnLRButtonClickedDel.AddDynamic(this, &ULRSettingPopupWidget::OnSaveButtonClicked);
	if (Btn_Default) Btn_Default->OnLRButtonClickedDel.AddDynamic(this, &ULRSettingPopupWidget::OnDefaultButtonClicked);
}

void ULRSettingPopupWidget::UnbindProperties()
{
	if (Btn_Close) Btn_Close->OnLRButtonClickedDel.Clear();
	if (Btn_Save) Btn_Save->OnLRButtonClickedDel.Clear();
	if (Btn_Default) Btn_Default->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRSettingPopupWidget::OnCloseButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->UndoUIHistory();
	//OnCloseUIRequestedDel.Broadcast(this);
}

void ULRSettingPopupWidget::OnSaveButtonClicked()
{
	SettingScrollWidget->SaveAllSettings();
}

void ULRSettingPopupWidget::OnDefaultButtonClicked()
{
	SettingScrollWidget->SetDefaultSettings();
}
