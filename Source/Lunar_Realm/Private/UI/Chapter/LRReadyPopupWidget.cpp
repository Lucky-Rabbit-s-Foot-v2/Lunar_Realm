// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRReadyPopupWidget.h"

#include "UI/Chapter/LRPartyLineupWidget.h"
#include "UI/Chapter/LRStageInfoWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

void ULRReadyPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_EmptyPoint) Btn_EmptyPoint->OnClicked.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnCloseRequested);
	if (Btn_Entrance) Btn_Entrance->OnClicked.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnEntranceButtonClicked);
	if (Btn_Close) Btn_Close->OnClicked.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnCloseRequested);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRReadyPopupWidget::UnbindProperties()
{
	OnCloseUIRequestedDel.Clear();

	if (Btn_EmptyPoint) Btn_EmptyPoint->OnClicked.Clear();
	if (Btn_Entrance) Btn_Entrance->OnClicked.Clear();
	if (Btn_Close) Btn_Close->OnClicked.Clear();

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
