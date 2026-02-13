// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRShopWidget.h"

#include "Components/Button.h"
#include "Units/LRControllerBase.h"

void ULRShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &ULRShopWidget::CloseButtonClicked);
	}

	if (ALRControllerBase* LRController = Cast<ALRControllerBase>(GetOwningPlayer()))
	{
		OnCloseUIRequested.AddDynamic(LRController, &ALRControllerBase::CloseWidget);
	}
}

void ULRShopWidget::NativeDestruct()
{
	if (Btn_Close)
	{
		Btn_Close->OnClicked.Clear();
	}
	Super::NativeDestruct();
}

void ULRShopWidget::CloseButtonClicked()
{
	OnCloseUIRequested.Broadcast(this);
}
