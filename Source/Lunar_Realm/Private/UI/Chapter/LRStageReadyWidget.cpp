// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageReadyWidget.h"

#include "UI/Chapter/LRPartyLineupWidget.h"
#include "UI/Chapter/LRStageInfoWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

void ULRStageReadyWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_EmptyPoint) Btn_EmptyPoint->OnClicked.AddUniqueDynamic(this, &ULRStageReadyWidget::OnCloseRequested);
	if (Btn_Entrance) Btn_Entrance->OnClicked.AddUniqueDynamic(this, &ULRStageReadyWidget::OnEntranceButtonClicked);
	if (Btn_Close) Btn_Close->OnClicked.AddUniqueDynamic(this, &ULRStageReadyWidget::OnCloseRequested);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRStageReadyWidget::UnbindProperties()
{
	OnCloseUIRequestedDel.Clear();

	if (Btn_EmptyPoint) Btn_EmptyPoint->OnClicked.Clear();
	if (Btn_Entrance) Btn_Entrance->OnClicked.Clear();
	if (Btn_Close) Btn_Close->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRStageReadyWidget::RefreshUI()
{
	Super::RefreshUI();
	PartyLineup->RefreshUI();
	StageInfo->RefreshUI();
}

void ULRStageReadyWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	SubWidgets.Add(PartyLineup);
	SubWidgets.Add(StageInfo);
}

void ULRStageReadyWidget::SetStageID(FName InStageID)
{
	StageID = InStageID;
	StageInfo->SetStageDataByID(InStageID);
}

void ULRStageReadyWidget::OnEntranceButtonClicked()
{
	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		GI->OpenNextStage(StageID);
	}
}

void ULRStageReadyWidget::OnCloseButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
