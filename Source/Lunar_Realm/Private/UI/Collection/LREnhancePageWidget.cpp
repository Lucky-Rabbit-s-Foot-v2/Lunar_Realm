// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhancePageWidget.h"

#include "Components/WidgetSwitcher.h"

#include "Engine/GameInstance.h"

#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterEnhanceWidget.h"
#include "UI/Collection/LREquipEnhance.h"

void ULREnhancePageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterEnhance);
	SubWidgets.Add(EquipEnhance);
	SubWidgets.Add(Collection);
}

void ULREnhancePageWidget::InitializeUI()
{
	Super::InitializeUI();
	if (ID.IsNone())
	{
		USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
		if (SaveGameSubsystem)
		{
			FName LeaderID = SaveGameSubsystem->GetLeaderCharacterID();
			SetCharacterID(LeaderID);
		}
	}
}

void ULREnhancePageWidget::SetCurrentTypeIndex(int32 InIndex)
{
	CurrentTypeIndex = InIndex;
	Switcher->SetActiveWidgetIndex(CurrentTypeIndex);
}

void ULREnhancePageWidget::SetMainID(FName InID)
{
	ID = InID;

	RefreshUI();
}

void ULREnhancePageWidget::SetCharacterID(const FName& InID)
{
	SetCurrentTypeIndex(0);

	CharacterEnhance->SetCharacterID(InID);
	SetMainID(InID);
}

void ULREnhancePageWidget::SetEquipID(const FName& InID)
{
	SetCurrentTypeIndex(1);

	EquipEnhance->SetEquipID(InID);
	SetMainID(InID);
}
