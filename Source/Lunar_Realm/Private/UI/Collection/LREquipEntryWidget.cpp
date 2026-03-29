// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Data/LRDataStructs.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"


void ULREquipEntryWidget::RefreshData()
{
	Super::RefreshData();
}

void ULREquipEntryWidget::OnTileClicked()
{
	SetType(ECollectionType::EQUIPMENT);

	Super::OnTileClicked();
}
