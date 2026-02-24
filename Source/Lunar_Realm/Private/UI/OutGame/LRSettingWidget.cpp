// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRSettingWidget.h"

#include "Components/Button.h"
#include "UI/OutGame/LRSettingScrollWidget.h"

void ULRSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &ULRSettingWidget::OnCloseButtonClicked);
	}
	if (Btn_Default)
	{
		Btn_Default->OnClicked.AddDynamic(this, &ULRSettingWidget::OnDefaultButtonClicked);
	}
}

void ULRSettingWidget::NativeDestruct()
{
	if (Btn_Close)
	{
		Btn_Close->OnClicked.Clear();
	}
	if (Btn_Default)
	{
		Btn_Default->OnClicked.Clear();
	}
	Super::NativeDestruct();
}

void ULRSettingWidget::OnCloseButtonClicked()
{
	OnCloseButtonClickedDel.Broadcast();
}

void ULRSettingWidget::OnDefaultButtonClicked()
{
	OnDefaultButtonClickedDel.Broadcast();
}


