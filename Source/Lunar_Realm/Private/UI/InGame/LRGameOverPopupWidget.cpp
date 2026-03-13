// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameOverPopupWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Core/Stage/LRStageGameMode.h"


void ULRGameOverPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Regroup) Btn_Regroup->OnClicked.AddDynamic(this, &ULRGameOverPopupWidget::OnRegroupButtonClicked);
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &ULRGameOverPopupWidget::OnRestartButtonClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &ULRGameOverPopupWidget::OnExitButtonClicked);
}

void ULRGameOverPopupWidget::UnbindProperties()
{
	if (Btn_Exit) Btn_Exit->OnClicked.Clear();
	if (Btn_Restart) Btn_Restart->OnClicked.Clear();
	if (Btn_Regroup) Btn_Regroup->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRGameOverPopupWidget::OnRegroupButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Regroup Button Clicked : Not implemented yet"));
}

void ULRGameOverPopupWidget::OnRestartButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnRestartGame();
}

void ULRGameOverPopupWidget::OnExitButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnExitStage();
}

void ULRGameOverPopupWidget::InitializeUI()
{
	Super::InitializeUI();

	if (Fail)
	{
		PlayAnimation(Fail);
	}

}
