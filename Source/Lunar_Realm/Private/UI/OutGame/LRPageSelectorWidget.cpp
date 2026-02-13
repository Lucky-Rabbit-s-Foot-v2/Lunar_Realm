// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRPageSelectorWidget.h"

#include "Components/Button.h"

#include "Units/OutGame/LROutGameController.h"

void ULRPageSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Stage)
	{
		Btn_Stage->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnStageButtonClicked);
	}

	if (Btn_Collection)
	{
		Btn_Collection->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnCollectionButtonClicked);
	}

	if (Btn_Party)
	{
		Btn_Party->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnPartyButtonClicked);
	}

	if (Btn_Gacha)
	{
		Btn_Gacha->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnGachaButtonClicked);
	}

	if(ALROutGameController* LRController = GetWorld()->GetFirstPlayerController<ALROutGameController>())
	{
		OnStageButtonClickedDel.AddDynamic(LRController, &ALROutGameController::OpenStageWidget);
		OnCollectionButtonClickedDel.AddDynamic(LRController, &ALROutGameController::OpenCollectionWidget);
		OnPartyButtonClickedDel.AddDynamic(LRController, &ALROutGameController::OpenPartyWidget);
		OnGachaButtonClickedDel.AddDynamic(LRController, &ALROutGameController::OpenGachaShopWidget);
	}
}

void ULRPageSelectorWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRPageSelectorWidget::OpenUI()
{
	Super::OpenUI();

	RefreshUI();

}

void ULRPageSelectorWidget::CloseUI()
{
	Super::CloseUI();
}

void ULRPageSelectorWidget::RefreshUI()
{
	Super::RefreshUI();
}

void ULRPageSelectorWidget::OnStageButtonClicked()
{
	OnStageButtonClickedDel.Broadcast();
}

void ULRPageSelectorWidget::OnCollectionButtonClicked()
{
	OnCollectionButtonClickedDel.Broadcast();
}

void ULRPageSelectorWidget::OnPartyButtonClicked()
{
	OnPartyButtonClickedDel.Broadcast();
}

void ULRPageSelectorWidget::OnGachaButtonClicked()
{
	OnGachaButtonClickedDel.Broadcast();
}

