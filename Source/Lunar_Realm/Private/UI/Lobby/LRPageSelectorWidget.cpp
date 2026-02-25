// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRPageSelectorWidget.h"

#include "Components/Button.h"

#include "Units/OutGame/LROutGameController.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Chapter/LRChapterSelectorWidget.h"

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
}

void ULRPageSelectorWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRPageSelectorWidget::OnStageButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUI<ULRChapterSelectorWidget>(ChapterSelectorWidgetClass);
}

void ULRPageSelectorWidget::OnCollectionButtonClicked()
{
}

void ULRPageSelectorWidget::OnPartyButtonClicked()
{
}

void ULRPageSelectorWidget::OnGachaButtonClicked()
{
}

