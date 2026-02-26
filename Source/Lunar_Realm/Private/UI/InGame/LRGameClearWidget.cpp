// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameClearWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Core/Stage/LRStageGameMode.h"


void ULRGameClearWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_NextStage) Btn_NextStage->OnClicked.AddDynamic(this, &ULRGameClearWidget::OnNextStageButtonClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &ULRGameClearWidget::OnExitButtonClicked);
}

void ULRGameClearWidget::UnbindProperties()
{
	if (Btn_NextStage) Btn_NextStage->OnClicked.Clear();
	if (Btn_Exit) Btn_Exit->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRGameClearWidget::OnNextStageButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnStartNextStage();
}

void ULRGameClearWidget::OnExitButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnExitStage();
}
