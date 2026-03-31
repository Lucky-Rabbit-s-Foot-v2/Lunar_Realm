// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRExitPopupWidget.h"

#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"

void ULRExitPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRExitPopupWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Exit->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRExitPopupWidget::OnExitButtonClicked);
	Btn_Back->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRExitPopupWidget::OnBackButtonClicked);
}

void ULRExitPopupWidget::UnbindProperties()
{
	Btn_Back->OnLRButtonClickedDel.Clear();
	Btn_Exit->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRExitPopupWidget::OnExitButtonClicked()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, true);
}

void ULRExitPopupWidget::OnBackButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
