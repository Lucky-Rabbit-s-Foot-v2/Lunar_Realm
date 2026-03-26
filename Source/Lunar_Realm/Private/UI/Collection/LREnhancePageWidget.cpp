// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhancePageWidget.h"

#include "Components/WidgetSwitcher.h"

#include "Engine/GameInstance.h"

#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterEnhanceWidget.h"
#include "UI/Collection/LREquipEnhance.h"

#include "Units/OutGame/LROutGameController.h"

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
			FSelectedInfo SelectedInfo(ECollectionType::CHARACTER, LeaderID);
			SetIDByType(SelectedInfo);
		}
	}
}

void ULREnhancePageWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);
	if (PC)
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULREnhancePageWidget::SetIDByType);
	}
}

void ULREnhancePageWidget::SetIDByType(const FSelectedInfo& InInfo)
{
	SwitchWidgetByType(InInfo.Type);
	ID = InInfo.ID;

	switch (InInfo.Type)
	{
	case ECollectionType::CHARACTER:
		CharacterEnhance->SetCharacterID(InInfo.ID);
		break;

	case ECollectionType::EQUIPMENT:
		EquipEnhance->SetEquipID(InInfo.ID);
		break;
	
	default:
		break;
	}
}

void ULREnhancePageWidget::SwitchWidgetByType(ECollectionType InType)
{
	Switcher->SetActiveWidgetIndex(static_cast<int32>(InType));
}
