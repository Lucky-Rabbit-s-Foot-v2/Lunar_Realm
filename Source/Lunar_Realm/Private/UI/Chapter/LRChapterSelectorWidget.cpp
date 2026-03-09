// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterSelectorWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"

#include "UI/Chapter/LRChapterWidget.h"
#include "UI/Lobby/LRLobbyWidget.h"

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

void ULRChapterSelectorWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	
	ChapterWidgets.Add(Chapter1);
	ChapterWidgets.Add(Chapter2);
	ChapterWidgets.Add(Chapter3);
}

void ULRChapterSelectorWidget::OnBackButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::LOBBY);
}
