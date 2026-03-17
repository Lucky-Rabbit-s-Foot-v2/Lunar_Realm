// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyPageWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Collection/LRPartySlotsWidget.h"
#include "UI/Collection/LRCollection.h"


void ULRPartyPageWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRPartyPageWidget::OnBackButtonClicked);
}

void ULRPartyPageWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPartyPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(PartySlot);
	SubWidgets.Add(Collection);
}

void ULRPartyPageWidget::OnBackButtonClicked()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::LOBBY);
	}
}
