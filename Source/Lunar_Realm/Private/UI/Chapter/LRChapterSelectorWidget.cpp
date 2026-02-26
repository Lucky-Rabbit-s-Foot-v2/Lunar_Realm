// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterSelectorWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"

#include "UI/Chapter/LRChapterWidget.h"
#include "UI/Lobby/LRLobbyWidget.h"

void ULRChapterSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ChapterWidgets.Empty();
	ChapterWidgets.Add(Chapter1);
	ChapterWidgets.Add(Chapter2);
	ChapterWidgets.Add(Chapter3);
}

void ULRChapterSelectorWidget::NativeDestruct()
{
	ChapterWidgets.Empty();
	
	Super::NativeDestruct();
}

void ULRChapterSelectorWidget::BindProperties()
{
	Super::BindProperties();
	
	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRChapterSelectorWidget::OnBackButtonClicked);
}

void ULRChapterSelectorWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();
	
	Super::UnbindProperties();
}

void ULRChapterSelectorWidget::RefreshUI()
{
	Super::RefreshUI();
	
	for (const auto& ChapterWidget : ChapterWidgets)
	{
		if (ChapterWidget)
		{
			LR_SCREEN_INFO(TEXT("Refreshing Chapter Widget: %s"), *ChapterWidget->GetName());
			ChapterWidget->RefreshUI();
		}
	}
}

void ULRChapterSelectorWidget::OnBackButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::LOBBY);
}
