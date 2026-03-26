// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyPageWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Collection/LRPartySlotsWidget.h"
#include "UI/Collection/LRCollection.h"

#include "Units/OutGame/LROutGameController.h"

void ULRPartyPageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer());
	if (PC)
	{
		OnPartyPageClosedDel.AddUniqueDynamic(PC, &ALROutGameController::OnPartyPageClosed);
	}
}

void ULRPartyPageWidget::NativeDestruct()
{
	OnPartyPageClosedDel.Clear();

	Super::NativeDestruct();
}

void ULRPartyPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(PartySlot);
	SubWidgets.Add(Collection);
}

void ULRPartyPageWidget::CloseUI()
{
	Super::CloseUI();

	OnPartyPageClosedDel.Broadcast();
}
