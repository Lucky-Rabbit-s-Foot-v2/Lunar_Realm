// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollectionPageWidget.h"

#include "Components/WidgetSwitcher.h"

#include "Units/OutGame/LROutGameController.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterInfoWidget.h"
#include "UI/Collection/LREquipmentInfo.h"

void ULRCollectionPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(EquipmentInfo);
	SubWidgets.Add(CharacterInfo);
	SubWidgets.Add(Collection);
}

void ULRCollectionPageWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);

	if (PC)
	{
		PC->OnSelectedCharacterDel.AddUniqueDynamic(this, &ULRCollectionPageWidget::SetCharacterID);
		PC->OnSelectedEquipmentDel.AddUniqueDynamic(this, &ULRCollectionPageWidget::SetEquipmentID);
	}
}

void ULRCollectionPageWidget::SetCharacterID(ESelectedType InType, FName InID)
{
	SwitchWidgetByType(InType);
	CharacterInfo->SetCharacterID(InID);
}

void ULRCollectionPageWidget::SetEquipmentID(ESelectedType InType, FName InID)
{
	SwitchWidgetByType(InType);
	EquipmentInfo->SetEquipID(InID);
}

void ULRCollectionPageWidget::SwitchWidgetByType(ESelectedType InType)
{
	Switcher->SetActiveWidgetIndex(static_cast<int32>(InType));
}
