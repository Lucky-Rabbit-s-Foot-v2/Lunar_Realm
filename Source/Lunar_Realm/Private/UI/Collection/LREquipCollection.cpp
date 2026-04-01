// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipCollection.h"

#include "Components/TextBlock.h"
#include "Components/TileView.h"

#include "Data/LRDataStructs.h"

#include "Engine/GameInstance.h"

#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Collection/LREquipEntryWidget.h"

void ULREquipCollection::RefreshUI()
{
	Super::RefreshUI();

	EquipTileView->ClearListItems();

	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();

	TArray<FName> AllEquipmentIDs = GameDataSubsystem->GetAllEquipmentIDs();

	for (const FName& EquipmentID : AllEquipmentIDs)
	{
		if (CollectionSubsystem->HasEquipment(EquipmentID))
		{
			AddItemToTileView(GameDataSubsystem, EquipmentID, false);
		}
	}

	for (const FName& EquipmentID : AllEquipmentIDs)
	{
		if (!CollectionSubsystem->HasEquipment(EquipmentID))
		{
			AddItemToTileView(GameDataSubsystem, EquipmentID, true);
		}
	}
}

void ULREquipCollection::AddItemToTileView(UGameDataSubsystem* GameDataSubsystem, const FName& LockedEquipID, bool bIsLocked)
{
	const FEquipmentStaticData& EquipmentData = GameDataSubsystem->GetEquipmentStaticData(LockedEquipID);
	const FGradeResourceData& GradeResourceData = GameDataSubsystem->GetGradeResourceData(EquipmentData.Grade);

	ULRTileData* TileDataObject = NewObject<ULRTileData>(this);
	TileDataObject->ID = EquipmentData.DataID;
	TileDataObject->Icon = EquipmentData.EquipmentTexture.LoadSynchronous();
	TileDataObject->bIsLocked = bIsLocked;
	TileDataObject->Frame = GradeResourceData.PortraitFrame.LoadSynchronous();

	EquipTileView->AddItem(TileDataObject);
}