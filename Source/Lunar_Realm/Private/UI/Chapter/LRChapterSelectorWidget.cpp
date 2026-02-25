// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterSelectorWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Lobby/LRLobbyWidget.h"

void ULRChapterSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &ULRChapterSelectorWidget::OnBackButtonClicked);
	}
}

void ULRChapterSelectorWidget::OnBackButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUI<ULRLobbyWidget>(LobbyWidgetClass);
}
