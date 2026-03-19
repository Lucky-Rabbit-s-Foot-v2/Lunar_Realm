// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPausePopupWidget.h"

#include "Core/LRGameInstance.h"
#include "Core/Stage/LRStageGameMode.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"

void ULRPausePopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Restart) Btn_Restart->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnRestartButtonClicked);
	if (Btn_Resume) Btn_Resume->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnResumeButtonClicked);
	if (Btn_Setting) Btn_Setting->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnSettingButtonClicked);
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnExitButtonClicked);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRPausePopupWidget::UnbindProperties()
{
	if (Btn_Restart) Btn_Restart->OnLRButtonClickedDel.Clear();
	if (Btn_Resume) Btn_Resume->OnLRButtonClickedDel.Clear();
	if (Btn_Setting) Btn_Setting->OnLRButtonClickedDel.Clear();
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRPausePopupWidget::OnRestartButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnRestartGame();
	}
}

void ULRPausePopupWidget::OnResumeButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
	
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnResumeGame();
	}
}

void ULRPausePopupWidget::OnSettingButtonClicked()
{
	if(UUIManagerSubsystem * UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::SETTING);
	}
}

void ULRPausePopupWidget::OnExitButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnExitStage();
	}
}