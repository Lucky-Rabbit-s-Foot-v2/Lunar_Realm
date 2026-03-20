// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameClearPopupWidget.h"

#include "Core/LRGameInstance.h"
#include "Core/Stage/LRStageGameMode.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"
#include "UI/InGame/LRStarBoxWidget.h"

void ULRGameClearPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.RemoveAll(this);
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRGameClearPopupWidget::NativeDestruct()
{
	OnCloseUIRequestedDel.Clear();

	Super::NativeDestruct();
}

void ULRGameClearPopupWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	
	SubWidgets.Add(StarBox);
}

void ULRGameClearPopupWidget::BindProperties()
{
	Super::BindProperties();

	Btn_NextStage->OnLRButtonClickedDel.RemoveAll(this);
	Btn_NextStage->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRGameClearPopupWidget::OnNextStageButtonClicked);
	
	Btn_Exit->OnLRButtonClickedDel.RemoveAll(this);
	Btn_Exit->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRGameClearPopupWidget::OnExitButtonClicked);
}

void ULRGameClearPopupWidget::UnbindProperties()
{
	
	Btn_NextStage->OnLRButtonClickedDel.Clear();
	
	Btn_Exit->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRGameClearPopupWidget::InitializeUI()
{
	Super::InitializeUI();
	if (victory)
	{
		PlayAnimation(victory);
	}
}

void ULRGameClearPopupWidget::SetIsLastStage(bool bInIsLastStage)
{
	bIsLastStage = bInIsLastStage;

	if (bIsLastStage)
	{
		Btn_NextStage->SetVisibility(ESlateVisibility::Collapsed);
		Btn_NextStage->SetIsEnabled(false);
	}
	else
	{
		Btn_NextStage->SetVisibility(ESlateVisibility::Visible);
		Btn_NextStage->SetIsEnabled(true);
	}
}

void ULRGameClearPopupWidget::SetStarMasking(int32 InMasking)
{
	StarBox->SetStarMasking(InMasking);
}

void ULRGameClearPopupWidget::OnNextStageButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);

	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnStartNextStage();
	}
}

void ULRGameClearPopupWidget::OnExitButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnExitStage();
	}
}
