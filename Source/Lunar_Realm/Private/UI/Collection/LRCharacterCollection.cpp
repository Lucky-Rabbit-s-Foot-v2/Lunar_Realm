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

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	TArray<FName> AllCharactersIDs = GameDataSubsystem->GetAllCharacterIDs();

	for (const FName& AllCharactersID : AllCharactersIDs)
	{
		const FCharacterStaticData& CharacterData = GameDataSubsystem->GetCharacterStaticData(AllCharactersID);
		ULRTileData* TileDataObject = NewObject<ULRTileData>(this);
		TileDataObject->SetID(CharacterData.DataID);
		TileDataObject->SetIcon(CharacterData.CharacterTexture.LoadSynchronous());
		CharacterTileView->AddItem(TileDataObject);
	}
}
