// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/LRSaveGame.h"

bool ULRSaveGame::HasCharacter(FName CharacterID) const
{
	return OwnedCharacters.Contains(CharacterID);
}

bool ULRSaveGame::HasEquipment(FName EquipmentID) const
{
	return OwnedEquipments.Contains(EquipmentID);
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

void ULRSaveGame::SetEquipmentSlot(int32 SlotIndex, FName EquipmentID)
{
	// 슬롯 수 보장
	while (SelectedEquipmentIDs.Num() <= SlotIndex)
	{
		SelectedEquipmentIDs.Add(NAME_None);
	}
        
	SelectedEquipmentIDs[SlotIndex] = EquipmentID;
}

FName ULRSaveGame::GetEquipmentID(int32 SlotIndex) const
{
	if (SelectedEquipmentIDs.IsValidIndex(SlotIndex))
	{
		return SelectedEquipmentIDs[SlotIndex];
	}
	
	LR_WARN(TEXT("Invalid SlotIndex"));
	return NAME_None;
}

TArray<FName> ULRSaveGame::GetAllEquippedIDs() const
{
	return SelectedEquipmentIDs;
}
