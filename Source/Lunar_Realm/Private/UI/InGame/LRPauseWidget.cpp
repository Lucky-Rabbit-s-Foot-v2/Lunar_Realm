// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPauseWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Core/Stage/LRStageGameMode.h"

void ULRPauseWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Restart) Btn_Restart->OnClicked.AddUniqueDynamic(this, &ULRPauseWidget::OnRestartButtonClicked);
	if (Btn_Resume) Btn_Resume->OnClicked.AddUniqueDynamic(this, &ULRPauseWidget::OnResumeButtonClicked);
	if (Btn_Setting) Btn_Setting->OnClicked.AddUniqueDynamic(this, &ULRPauseWidget::OnSettingButtonClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddUniqueDynamic(this, &ULRPauseWidget::OnExitButtonClicked);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRPauseWidget::UnbindProperties()
{
	if (Btn_Restart) Btn_Restart->OnClicked.Clear();
	if (Btn_Resume) Btn_Resume->OnClicked.Clear();
	if (Btn_Setting) Btn_Setting->OnClicked.Clear();
	if (Btn_Exit) Btn_Exit->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPauseWidget::OnRestartButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnRestartGame();
}

void ULRPauseWidget::OnResumeButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
	
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnResumeGame();
}

void ULRPauseWidget::OnSettingButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::SETTING);
}

void ULRPauseWidget::OnExitButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnExitStage();
}