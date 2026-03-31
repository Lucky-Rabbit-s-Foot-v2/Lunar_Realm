// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRGameOverPopupWidget.h"

#include "Core/LRGameInstance.h"
#include "Core/Stage/LRStageGameMode.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"
#include "UI/InGame/LRStarBoxWidget.h"
#include "UI/InGame/LRExpPanelWidget.h"

void ULRGameOverPopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Regroup) Btn_Regroup->OnLRButtonClickedDel.AddDynamic(this, &ULRGameOverPopupWidget::OnRegroupButtonClicked);
	if (Btn_Restart) Btn_Restart->OnLRButtonClickedDel.AddDynamic(this, &ULRGameOverPopupWidget::OnRestartButtonClicked);
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.AddDynamic(this, &ULRGameOverPopupWidget::OnExitButtonClicked);
}

void ULRGameOverPopupWidget::UnbindProperties()
{
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.Clear();
	if (Btn_Restart) Btn_Restart->OnLRButtonClickedDel.Clear();
	if (Btn_Regroup) Btn_Regroup->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRGameOverPopupWidget::OnRegroupButtonClicked()
{
	LR_SCREEN_INFO(TEXT("Regroup Button Clicked : Not implemented yet"));
}

void ULRGameOverPopupWidget::OnRestartButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnRestartGame();
	}
}

void ULRGameOverPopupWidget::OnExitButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnExitStage();
	}
}

void ULRGameOverPopupWidget::InitializeUI()
{
	Super::InitializeUI();

	if (Fail)
	{
		PlayAnimation(Fail);
	}
	if (LeaderExpPanel) LeaderExpPanel->SetVisibility(ESlateVisibility::Collapsed);
	if (MemberExpPanel) MemberExpPanel->SetVisibility(ESlateVisibility::Collapsed);

}

void ULRGameOverPopupWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation == Fail)
	{
		LR_INFO(TEXT("Fail 애니메이션 종료! 패배 보상(20%%) 경험치 패널 표시 명령"));
		ShowExpPanel2();
	}
}

void ULRGameOverPopupWidget::ShowExpPanel2()
{
	if (LeaderExpPanel)
	{
		LeaderExpPanel->SetVisibility(ESlateVisibility::Visible);
		LeaderExpPanel->SetupExpPanel(false, 1);
	}

	if (MemberExpPanel)
	{
		MemberExpPanel->SetVisibility(ESlateVisibility::Visible);
		MemberExpPanel->SetupExpPanel(false, 2);
	}
}