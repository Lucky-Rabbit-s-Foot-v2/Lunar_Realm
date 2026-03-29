// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyPageWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRPartySlotsWidget.h"
#include "UI/Collection/LRCollection.h"

#include "Units/OutGame/LROutGameController.h"


void ULRPartyPageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SaveGameSubsystem->OnSaveGameSavedDel.AddUniqueDynamic(this, &ULRPartyPageWidget::RefreshUICaller);
	}
}

void ULRPartyPageWidget::NativeDestruct()
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SaveGameSubsystem->OnSaveGameSavedDel.RemoveDynamic(this, &ULRPartyPageWidget::RefreshUICaller);
	}

	Super::NativeDestruct();
}

void ULRPartyPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(PartySlot);
	SubWidgets.Add(Collection);
}

void ULRPartyPageWidget::RefreshUICaller()
{
	RefreshUI();
}

void ULRPartyPageWidget::SetIDAndType(FName InID, ECollectionType InType)
{
	PartySlot->SetIDAndType(InID, InType);
}