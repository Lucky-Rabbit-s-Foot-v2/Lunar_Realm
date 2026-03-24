// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Data/LRDataStructs.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULREquipEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		OnEquipTileClickedDel.AddUniqueDynamic(PC, &ALROutGameController::SetSelectedEquipmentID);
	}
}

void ULREquipEntryWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Selected->OnClicked.AddUniqueDynamic(this, &ULREquipEntryWidget::OnTileClicked);
}

void ULREquipEntryWidget::UnbindProperties()
{
	Btn_Selected->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULREquipEntryWidget::RefreshData()
{
	Super::RefreshData();

	if (TileData->IsLocked())
	{
		Btn_Selected->SetIsEnabled(false);
	}
	else
	{
		Btn_Selected->SetIsEnabled(true);
	}
}

void ULREquipEntryWidget::OnTileClicked()
{
	OnEquipTileClickedDel.Broadcast(TileData->GetID());
}
