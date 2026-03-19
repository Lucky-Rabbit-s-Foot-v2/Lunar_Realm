// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameClearPopupWidget.h"

#include "Core/LRGameInstance.h"
#include "Core/Stage/LRStageGameMode.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"

void ULRGameClearPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_NextStage) Btn_NextStage->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRGameClearPopupWidget::OnNextStageButtonClicked);
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRGameClearPopupWidget::OnExitButtonClicked);
}

void ULRGameClearPopupWidget::UnbindProperties()
{
	if (Btn_NextStage) Btn_NextStage->OnLRButtonClickedDel.Clear();
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.Clear();

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

void ULRGameClearPopupWidget::OnNextStageButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnStartNextStage();
	}
}

void ULRGameClearPopupWidget::OnExitButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnExitStage();
	}
}
