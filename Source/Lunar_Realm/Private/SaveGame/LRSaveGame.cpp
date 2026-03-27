// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/LRSaveGame.h"

#include "Subsystems/GameDataSubsystem.h"

ULRSaveGame::ULRSaveGame(const FObjectInitializer& InitializerModule)
{
	SelectedCharactersIDs.SetNum(PARTY_SLOT_SIZE);
	for (int32 i = 0; i < PARTY_SLOT_SIZE; ++i)
	{
		SelectedCharactersIDs[i] = NAME_None;
	}

	SelectedEquipmentIDs.SetNum(EQUIPMENT_SLOT_SIZE);
	for(int32 i = 0; i < EQUIPMENT_SLOT_SIZE; ++i)
	{
		SelectedEquipmentIDs[i] = FGuid();
	}
}


bool ULRSaveGame::HasCharacter(FName CharacterID) const
{
	return OwnedCharacters.Contains(CharacterID);
}

bool ULRSaveGame::HasEquipment(FName EquipmentID) const
{
	return OwnedEquipments.ContainsByPredicate([EquipmentID](const FEquipmentInstance& Instance)
	{
		return Instance.EquipmentID == EquipmentID;
	});
}

void ULRSaveGame::SetCharacterPartySlot(int32 SlotIndex, FName CharacterID)
{
	//슬롯 수 보장
	while (SelectedCharactersIDs.Num() <= SlotIndex)
	{
		SelectedCharactersIDs.Add(NAME_None);
	}
	
	SelectedCharactersIDs[SlotIndex] = CharacterID;
}

FName ULRSaveGame::GetCharacterID(int32 SlotIndex) const
{
	if (SelectedCharactersIDs.IsValidIndex(SlotIndex))
	{
		return SelectedCharactersIDs[SlotIndex];
	}
	
	LR_WARN(TEXT("Invalid SlotIndex"));
	return NAME_None;
}

FName ULRSaveGame::GetLeaderCharacterID() const
{
	return GetCharacterID(0);
}

TArray<FName> ULRSaveGame::GetAllCharacterSlots() const
{
	return SelectedCharactersIDs;
}

void ULRSaveGame::SetEquipmentSlot(int32 SlotIndex, FGuid EquipmentID)
{
	// 슬롯 수 보장
	while (SelectedEquipmentIDs.Num() <= SlotIndex)
	{
		SelectedEquipmentIDs.SetNum(EQUIPMENT_SLOT_SIZE);
	}
        
	SelectedEquipmentIDs[SlotIndex] = EquipmentID;
}

FGuid ULRSaveGame::GetEquipmentID(int32 SlotIndex) const
{
	if (SelectedEquipmentIDs.IsValidIndex(SlotIndex))
	{
		return SelectedEquipmentIDs[SlotIndex];
	}
	
	LR_WARN(TEXT("Invalid SlotIndex"));
	return FGuid();
}

TArray<FGuid> ULRSaveGame::GetLeadersEquippedGuids() const
{
	return SelectedEquipmentIDs;
}

TMap<FName, FCharacterInstance> ULRSaveGame::GetOwnedCharactersList() const
{
	return OwnedCharacters;
}

TArray<FEquipmentInstance> ULRSaveGame::GetOwnedEquipmentsList() const
{
	return OwnedEquipments;
}

void ULRSaveGame::SetOwnedCharactersList(TMap<FName, FCharacterInstance> InCharactersList)
{
	if (InCharactersList.IsEmpty())
	{
		LR_WARN(TEXT("Invalid CharacterMap"));
		return;
	}
	
	OwnedCharacters = InCharactersList;
}

void ULRSaveGame::SetOwnedEquipmentsList(TArray<FEquipmentInstance> InEquipmentList)
{
	if (InEquipmentList.IsEmpty())
	{
		LR_WARN(TEXT("Invalid EquipmentMap"));
		return;
	}
	
	OwnedEquipments = InEquipmentList;
}

void ULRSaveGame::InitializeNewPlayerDefaults()
{
	Gold = 1000;
	CrescentTicket = 1000;
	FullMoonTicket = 1000;
	LastUpdatedUtc = FDateTime::UtcNow();

	SetCharacterPartySlot(0, FName("Army_N"));
	
	TMap<FName, FCharacterInstance> DefaultCharacters;
	FCharacterInstance DefaultCharacterInstance(FName("Army_N"), 1);
	DefaultCharacters.Add(FName("Army_N"), DefaultCharacterInstance);
	SetOwnedCharactersList(DefaultCharacters);

}

TMap<FName, FStageClearedData> ULRSaveGame::GetStageClearedDataList() const
{
	return StageClearedDataMap;
}

FStageClearedData ULRSaveGame::GetStageClearedDataMap(FName InID)
{
	return StageClearedDataMap.Find(InID) ? *StageClearedDataMap.Find(InID) : FStageClearedData();
}

void ULRSaveGame::UpdateStageClearedData(FName InStageID, FStageClearedData& InData)
{
	StageClearedDataMap.Add(InStageID, InData);
}
