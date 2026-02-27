// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageReadyWidget.h"

#include "UI/Chapter/LRPartyLineupWidget.h"
#include "UI/Chapter/LRStageInfoWidget.h"

#include "Components/Button.h"

void ULRStageReadyWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Entrance) Btn_Entrance->OnClicked.AddDynamic(this, &ULRStageReadyWidget::OnCloseRequested);
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

void ULRStageReadyWidget::SetStageDataByID(FName InStageID)
{
	StageInfo->SetStageDataByID(InStageID);
}

void ULRStageReadyWidget::OnEntranceButtonClicked()
{
	// TODO: 스테이지 입장 처리
}

void ULRStageReadyWidget::OnCloseButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
