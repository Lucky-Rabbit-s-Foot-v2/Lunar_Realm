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
		PC->OnSelectedCharacterChangedDel.AddUniqueDynamic(this, &ULRCollectionPageWidget::SetCharacterID);
		PC->OnSelectedEquipmentChangedDel.AddUniqueDynamic(this, &ULRCollectionPageWidget::SetEquipmentID);
	}
}

void ULRCollectionPageWidget::SetCharacterID(FName InID)
{
	Switcher->SetActiveWidgetIndex(0);
	CharacterInfo->SetCharacterID(InID);
}

void ULRCollectionPageWidget::SetEquipmentID(FName InID)
{
	Switcher->SetActiveWidgetIndex(1);
	EquipmentInfo->SetEquipID(InID);
}