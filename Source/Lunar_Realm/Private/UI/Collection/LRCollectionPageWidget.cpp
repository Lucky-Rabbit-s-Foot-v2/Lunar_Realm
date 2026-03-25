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
		SetIDByType(ESelectedType::CHARACTER, LeaderID);
		
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

void ULRCollectionPageWidget::SetIDByType(ESelectedType InType, FName InID)
{
	SwitchWidgetByType(InType);

	if (InType == ESelectedType::CHARACTER)
	{
		CharacterInfo->SetCharacterID(InID);
	}
	else if (InType == ESelectedType::EQUIPMENT)
	{
		EquipmentInfo->SetEquipID(InID);
	}
}

void ULRCollectionPageWidget::SwitchWidgetByType(ESelectedType InType)
{
	Switcher->SetActiveWidgetIndex(static_cast<int32>(InType));
}
