// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRPageSelectorWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Units/OutGame/LROutGameController.h"
#include "UI/Chapter/LRChapterPageWidget.h"

void ULRPageSelectorWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Stage) Btn_Stage->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnStageButtonClicked);
	if (Btn_Collection) Btn_Collection->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnCollectionButtonClicked);
	if (Btn_Party) Btn_Party->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnPartyButtonClicked);
	if (Btn_Gacha) Btn_Gacha->OnClicked.AddDynamic(this, &ULRPageSelectorWidget::OnGachaButtonClicked);
}

void ULRPageSelectorWidget::UnbindProperties()
{
	if (Btn_Stage) Btn_Stage->OnClicked.Clear();
	if (Btn_Collection) Btn_Collection->OnClicked.Clear();
	if (Btn_Party) Btn_Party->OnClicked.Clear();
	if (Btn_Gacha) Btn_Gacha->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPageSelectorWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Profile);
}

void ULRPageSelectorWidget::OnStageButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	
	ULRChapterPageWidget* ChapterSelector = Cast<ULRChapterPageWidget>(UIManager->SwitchPageUIByID(EUIID::DUTY));
	ChapterSelector->RefreshUI();
}

void ULRPageSelectorWidget::OnCollectionButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::COLLECTION);
}

void ULRPageSelectorWidget::OnPartyButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::PARTY);
}

void ULRPageSelectorWidget::OnGachaButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::GACHA);
}
