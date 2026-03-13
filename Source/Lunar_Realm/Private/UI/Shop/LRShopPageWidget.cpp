// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Shop/LRShopPageWidget.h"

#include "Components/Button.h"
#include "Units/LRControllerBase.h"

void ULRShopPageWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Close) Btn_Close->OnClicked.AddDynamic(this, &ULRShopPageWidget::CloseButtonClicked);
}

void ULRShopPageWidget::UnbindProperties()
{
	if (Btn_Close) Btn_Close->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRShopPageWidget::CloseButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::LOBBY);
}
