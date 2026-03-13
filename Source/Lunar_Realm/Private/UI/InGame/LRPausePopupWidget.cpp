// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPausePopupWidget.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Core/Stage/LRStageGameMode.h"

void ULRPausePopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Restart) Btn_Restart->OnClicked.AddUniqueDynamic(this, &ULRPausePopupWidget::OnRestartButtonClicked);
	if (Btn_Resume) Btn_Resume->OnClicked.AddUniqueDynamic(this, &ULRPausePopupWidget::OnResumeButtonClicked);
	if (Btn_Setting) Btn_Setting->OnClicked.AddUniqueDynamic(this, &ULRPausePopupWidget::OnSettingButtonClicked);
	if (Btn_Exit) Btn_Exit->OnClicked.AddUniqueDynamic(this, &ULRPausePopupWidget::OnExitButtonClicked);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRPausePopupWidget::UnbindProperties()
{
	if (Btn_Restart) Btn_Restart->OnClicked.Clear();
	if (Btn_Resume) Btn_Resume->OnClicked.Clear();
	if (Btn_Setting) Btn_Setting->OnClicked.Clear();
	if (Btn_Exit) Btn_Exit->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPausePopupWidget::OnRestartButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnRestartGame();
}

void ULRPausePopupWidget::OnResumeButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
	
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnResumeGame();
}

void ULRPausePopupWidget::OnSettingButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::SETTING);
}

void ULRPausePopupWidget::OnExitButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode());
	StageGM->OnExitStage();
}