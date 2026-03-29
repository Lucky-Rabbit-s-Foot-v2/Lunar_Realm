// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterCollection.h"

#include "Components/TextBlock.h"
#include "Components/TileView.h"

#include "Data/LRDataStructs.h"
#include "Engine/GameInstance.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Collection/LRCharacterEntryWidget.h"

void ULRCharacterCollection::RefreshUI()
{
	Super::RefreshUI();

	CharacterTileView->ClearListItems();

	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	
	TArray<FName> UnlockedCharactersIDs = CollectionSubsystem->GetUnlockedCharacterIDs();
	for (const FName& UnlockedCharactersID : UnlockedCharactersIDs)
	{
		AddItemToTileView(GameDataSubsystem, UnlockedCharactersID, false);
	}
	
	TArray<FName> AllCharactersIDs = CollectionSubsystem->GetAllCharacterIDs();
	TArray<FName> LockedCharactersIDs = AllCharactersIDs.FilterByPredicate(
		[&](const FName& ID)
		{
			return !UnlockedCharactersIDs.Contains(ID);
		}
	);
	
	for(const FName& LockedCharactersID : LockedCharactersIDs)
	{
		AddItemToTileView(GameDataSubsystem, LockedCharactersID, true);
	}
}

void ULRCharacterCollection::AddItemToTileView(UGameDataSubsystem* GameDataSubsystem, const FName& LockedCharactersID, bool bIsLocked)
{
	const FCharacterStaticData& CharacterData = GameDataSubsystem->GetCharacterStaticData(LockedCharactersID);
	ULRTileData* TileDataObject = NewObject<ULRTileData>(this);

	TileDataObject->ID = CharacterData.DataID;
	TileDataObject->Icon = CharacterData.CharacterTexture.LoadSynchronous();
	TileDataObject->Frame = CharacterData.PortraitFrame.LoadSynchronous();
	TileDataObject->bIsLocked = bIsLocked;

	CharacterTileView->AddItem(TileDataObject);
}
