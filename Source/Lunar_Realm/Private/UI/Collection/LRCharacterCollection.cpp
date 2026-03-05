// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterCollection.h"

#include "Components/TextBlock.h"
#include "Components/TileView.h"

#include "Data/LRDataStructs.h"
#include "Engine/GameInstance.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"


#include "UI/Collection/LRCharacterEntryWidget.h"

void ULRCharacterCollection::InitializeUI()
{
	Super::InitializeUI();

	/*UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	TArray<FName> CharacterIDs = CollectionSubsystem->GetAllCharacterIDs();

	LR_SCREEN_INFO(TEXT("Initializing Character Collection UI with %d characters"), CharacterIDs.Num());

	for (const FName& CharacterID : CharacterIDs)
	{
		ULRCharacterEntryWidget* TileData = NewObject<ULRCharacterEntryWidget>(this);
		TileData->SetData(CharacterID);
		CharacterTileView->AddItem(TileData);
	}

	TArray<FName> UnlockedCharacterIDs = CollectionSubsystem->GetUnlockedCharacterIDs();
	LR_SCREEN_INFO(TEXT("Initializing Character Collection UI with %d characters"), UnlockedCharacterIDs.Num());

	for (const FName& UnlockedCharacterID : UnlockedCharacterIDs)
	{
		ULRCharacterEntryWidget* TileData = NewObject<ULRCharacterEntryWidget>(this);
		TileData->SetData(UnlockedCharacterID);
		CharacterTileView->AddItem(TileData);
	}*/

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	TArray<FName> AllCharactersIDs = GameDataSubsystem->GetAllCharacterIDs();
	LR_SCREEN_INFO(TEXT("Initializing Character Collection UI with %d characters"), AllCharactersIDs.Num());

	for (const FName& AllCharactersID : AllCharactersIDs)
	{
		const FCharacterStaticData& CharacterData = GameDataSubsystem->GetCharacterStaticData(AllCharactersID);
		ULRTileData* TileDataObject = NewObject<ULRTileData>(this);
		TileDataObject->SetID(CharacterData.DataID);
		TileDataObject->SetIcon(CharacterData.PortraitIcon.LoadSynchronous());
		CharacterTileView->AddItem(TileDataObject);
	}
}

void ULRCharacterCollection::RefreshUI()
{
	Super::RefreshUI();

	if (CharacterTileView)
	{
		CharacterTileView->RegenerateAllEntries();
	}
}
