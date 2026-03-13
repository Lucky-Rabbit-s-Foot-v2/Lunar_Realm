// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameClearPopupWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Core/Stage/LRStageGameMode.h"


void ULRGameClearPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_NextStage) Btn_NextStage->OnClicked.AddDynamic(this, &ULRGameClearPopupWidget::OnNextStageButtonClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &ULRGameClearPopupWidget::OnExitButtonClicked);
}

void ULRGameClearPopupWidget::UnbindProperties()
{
	if (Btn_NextStage) Btn_NextStage->OnClicked.Clear();
	if (Btn_Exit) Btn_Exit->OnClicked.Clear();

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
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnStartNextStage();
}

void ULRGameClearPopupWidget::OnExitButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnExitStage();
}
