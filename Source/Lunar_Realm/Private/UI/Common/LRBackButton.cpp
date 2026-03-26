// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRBackButton.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

void ULRBackButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Hidden);
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	//UIManager->OnHistoryChangedDel.AddUniqueDynamic(this, &ULRBackButton::SetVisibilityByUIHistory);
}

void ULRBackButton::NativeDestruct()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OnHistoryChangedDel.RemoveAll(this);

	Super::NativeDestruct();
}

void ULRBackButton::OnButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->UndoUIHistory();
}

void ULRBackButton::SetVisibilityByUIHistory()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (UIManager)
	{
		if (UIManager->GetUIHistoryStackSize() > 1)
		{
			SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
