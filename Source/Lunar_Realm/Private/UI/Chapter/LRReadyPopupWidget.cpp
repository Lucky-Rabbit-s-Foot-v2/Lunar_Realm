// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRReadyPopupWidget.h"

#include "UI/Chapter/LRPartyLineupWidget.h"
#include "UI/Chapter/LRStageInfoWidget.h"

#include "UI/Core/LRButtonWidget.h"

#include "Core/LRGameInstance.h"

#include "Components/Button.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

void ULRReadyPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_EmptyPoint) Btn_EmptyPoint->OnClicked.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnCloseRequested);

	if (Btn_Party) Btn_Party->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnPartyButtonClicked);
	if (Btn_Entrance) Btn_Entrance->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnEntranceButtonClicked);
	if (Btn_Close) Btn_Close->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnCloseRequested);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRReadyPopupWidget::UnbindProperties()
{
	OnCloseUIRequestedDel.Clear();

	if (Btn_EmptyPoint) Btn_EmptyPoint->OnClicked.Clear();

	if (Btn_Party) Btn_Party->OnLRButtonClickedDel.Clear();
	if (Btn_Entrance) Btn_Entrance->OnLRButtonClickedDel.Clear();
	if (Btn_Close) Btn_Close->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRReadyPopupWidget::RefreshUI()
{
	Super::RefreshUI();
	PartyLineup->RefreshUI();
	StageInfo->RefreshUI();
}

void ULRReadyPopupWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	SubWidgets.Add(PartyLineup);
	SubWidgets.Add(StageInfo);
}

void ULRReadyPopupWidget::SetStageID(FName InStageID)
{
	StageID = InStageID;
	StageInfo->SetStageDataByID(InStageID);
}

void ULRReadyPopupWidget::OnPartyButtonClicked()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::PARTY);
	}
}

void ULRReadyPopupWidget::OnEntranceButtonClicked()
{
	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		GI->OpenNextStage(StageID);
	}
}

void ULRReadyPopupWidget::OnCloseButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
