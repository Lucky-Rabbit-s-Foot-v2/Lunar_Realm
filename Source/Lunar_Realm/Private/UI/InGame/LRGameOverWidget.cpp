// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameOverWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Core/Stage/LRStageGameMode.h"


void ULRGameOverWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Regroup) Btn_Regroup->OnClicked.AddDynamic(this, &ULRGameOverWidget::OnRegroupButtonClicked);
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &ULRGameOverWidget::OnRestartButtonClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddDynamic(this, &ULRGameOverWidget::OnExitButtonClicked);
}

void ULRGameOverWidget::UnbindProperties()
{
	if (Btn_Exit) Btn_Exit->OnClicked.Clear();
	if (Btn_Restart) Btn_Restart->OnClicked.Clear();
	if (Btn_Regroup) Btn_Regroup->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRGameOverWidget::OnRegroupButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Regroup Button Clicked : Not implemented yet"));
}

void ULRGameOverWidget::OnRestartButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnRestartGame();
}

void ULRGameOverWidget::OnExitButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnExitStage();
}

void ULRGameOverWidget::InitializeUI()
{
	Super::InitializeUI();

	if (Fail)
	{
		PlayAnimation(Fail);
	}

}
