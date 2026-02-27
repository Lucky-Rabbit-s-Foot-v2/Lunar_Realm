// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageReadyWidget.h"

#include "UI/Chapter/LRPartyLineupWidget.h"
#include "UI/Chapter/LRStageInfoWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"

void ULRStageReadyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		OnStageOpenClickedDel.AddDynamic(GI, &ULRGameInstance::OpenNextStage);
	}
}

void ULRStageReadyWidget::NativeDestruct()
{
	OnStageOpenClickedDel.Clear();

	Super::NativeDestruct();
}

void ULRStageReadyWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Entrance) Btn_Entrance->OnClicked.AddDynamic(this, &ULRStageReadyWidget::OnEntranceButtonClicked);
	if (Btn_Close) Btn_Close->OnClicked.AddDynamic(this, &ULRStageReadyWidget::OnCloseRequested);
}

void ULRStageReadyWidget::UnbindProperties()
{
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

void ULRStageReadyWidget::SetStageID(FName InStageID)
{
	StageID = InStageID;
	StageInfo->SetStageDataByID(InStageID);
}

void ULRStageReadyWidget::OnEntranceButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Stage Entrance Button Clicked. StageID: %s"), *StageID.ToString());
	OnStageOpenClickedDel.Broadcast(StageID);
}

void ULRStageReadyWidget::OnCloseButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
