// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRReadyPopupWidget.h"

#include "UI/Chapter/LRPartyLineupWidget.h"
#include "UI/Chapter/LRStageInfoWidget.h"
#include "UI/Chapter/LRStagePageWidget.h"

#include "UI/Core/LRButtonWidget.h"

#include "Core/LRGameInstance.h"

#include "Components/Button.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

void ULRReadyPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRReadyPopupWidget::NativeDestruct()
{
	OnCloseUIRequestedDel.Clear();

	Super::NativeDestruct();
}

void ULRReadyPopupWidget::BindProperties()
{
	Super::BindProperties();

	Btn_EmptyPoint->OnClicked.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnCloseRequested);

	Btn_Party->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnPartyButtonClicked);
	Btn_Entrance->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnEntranceButtonClicked);
	Btn_Close->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRReadyPopupWidget::OnCloseRequested);
}

void ULRReadyPopupWidget::UnbindProperties()
{
	Btn_EmptyPoint->OnClicked.Clear();

	Btn_Party->OnLRButtonClickedDel.Clear();
	Btn_Entrance->OnLRButtonClickedDel.Clear();
	Btn_Close->OnLRButtonClickedDel.Clear();

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

void ULRReadyPopupWidget::OpenUI()
{
	if (Anim_Falling)
	{
		//PlayAnimation(Anim_Falling);
	}
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
	LR_SCREEN_INFO(TEXT("ReadyPopupWidget: Party Button Clicked, StageID: %s"), *StageID.ToString());
	OnCloseUIRequestedDel.Broadcast(this);
}

void ULRReadyPopupWidget::OnEntranceButtonClicked()
{
	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		GI->OpenNextStage(StageID);
	}
	LR_SCREEN_INFO(TEXT("ReadyPopupWidget: Entrance Button Clicked, StageID: %s"), *StageID.ToString());
	OnCloseUIRequestedDel.Broadcast(this);
}

void ULRReadyPopupWidget::OnCloseButtonClicked()
{
	LR_SCREEN_INFO(TEXT("ReadyPopupWidget: Close Button Clicked"));
	OnCloseUIRequestedDel.Broadcast(this);
}
