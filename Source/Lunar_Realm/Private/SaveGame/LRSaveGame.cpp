// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/LRSaveGame.h"

ULRSaveGame::ULRSaveGame(const FObjectInitializer& InitializerModule)
{
	SelectedCharactersIDs.SetNum(PARTY_SLOT_SIZE);
	SelectedEquipmentIDs.SetNum(EQUIPMENT_SLOT_SIZE);
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
}
