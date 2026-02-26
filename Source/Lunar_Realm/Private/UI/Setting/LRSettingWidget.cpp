// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LRSettingWidget.h"

#include "Components/Button.h"
#include "UI/Setting/LRSettingScrollWidget.h"

void ULRSettingWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Close) Btn_Close->OnClicked.AddDynamic(this, &ULRSettingWidget::OnCloseButtonClicked);
	if (Btn_Save) Btn_Save->OnClicked.AddDynamic(this, &ULRSettingWidget::OnSaveButtonClicked);
	if (Btn_Default) Btn_Default->OnClicked.AddDynamic(this, &ULRSettingWidget::OnDefaultButtonClicked);
}

void ULRSettingWidget::UnbindProperties()
{
	if (Btn_Close) Btn_Close->OnClicked.Clear();
	if (Btn_Save) Btn_Save->OnClicked.Clear();
	if (Btn_Default) Btn_Default->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRSettingWidget::OnCloseButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}

void ULRSettingWidget::OnSaveButtonClicked()
{
	SettingScrollWidget->SaveAllSettings();
}

void ULRSettingWidget::OnDefaultButtonClicked()
{
	SettingScrollWidget->SetDefaultSettings();
}
