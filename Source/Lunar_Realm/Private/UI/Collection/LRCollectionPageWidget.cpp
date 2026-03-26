// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollectionPageWidget.h"

#include "Components/WidgetSwitcher.h"

#include "Units/OutGame/LROutGameController.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterInfoWidget.h"
#include "UI/Collection/LREquipmentInfo.h"

#include "Engine/GameInstance.h"
#include "Subsystems/SaveGameSubsystem.h"

void ULRCollectionPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(EquipmentInfo);
	SubWidgets.Add(CharacterInfo);
	SubWidgets.Add(Collection);
}

void ULRCollectionPageWidget::InitializeUI()
{
	Super::InitializeUI();

	if (ID.IsNone())
	{
		USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
		if (!SaveGameSubsystem)
		{
			return;
		}
		
		FName LeaderID = SaveGameSubsystem->GetLeaderCharacterID();
		FSelectedInfo SelectedInfo(ECollectionType::CHARACTER, LeaderID);
		SetIDByType(SelectedInfo);
		
		ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer());
		if (PC)
		{
			PC->SetSelectedCharacterID(LeaderID);
		}
	}
}

void ULRCollectionPageWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);

	if (PC)
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULRCollectionPageWidget::SetIDByType);
	}
}

void ULRCollectionPageWidget::SetIDByType(const FSelectedInfo& InInfo)
{
	SwitchWidgetByType(InInfo.Type);
	ID = InInfo.ID;

	switch (InInfo.Type)
	{
	case ECollectionType::CHARACTER:
		CharacterInfo->SetCharacterID(InInfo.ID);
		break;

	case ECollectionType::EQUIPMENT:
		EquipmentInfo->SetEquipID(InInfo.ID);
		break;

	default:
		break;
	}
}

void ULRCollectionPageWidget::SwitchWidgetByType(ECollectionType InType)
{
	Switcher->SetActiveWidgetIndex(static_cast<int32>(InType));
}
