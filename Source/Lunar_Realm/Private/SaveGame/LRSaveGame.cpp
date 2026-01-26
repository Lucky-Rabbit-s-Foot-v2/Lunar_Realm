// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/LRSaveGame.h"

bool ULRSaveGame::HasCharacter(int32 CharacterID) const
{
	return OwnedCharacters.Contains(CharacterID);
}

bool ULRSaveGame::HasEquipment(int32 EquipmentID) const
{
	return OwnedEquipments.Contains(EquipmentID);
}

void ULRSaveGame::SetCharacterSlot(int32 SlotIndex, int32 CharacterID)
{
	//슬롯 수 보장
	while (SelectedCharacterIDs.Num() <= SlotIndex)
	{
		SelectedCharacterIDs.Add(-1);
	}
	
	SelectedCharacterIDs[SlotIndex] = CharacterID;
}

int32 ULRSaveGame::GetCharacterSlot(int32 SlotIndex) const
{
	if (SelectedCharacterIDs.IsValidIndex(SlotIndex))
	{
		return SelectedCharacterIDs[SlotIndex];
	}
	
	return -1;
}

TArray<int32> ULRSaveGame::GetAllCharacterSlots() const
{
	return SelectedCharacterIDs;
}

void ULRSaveGame::SetEquipmentSlot(int32 SlotIndex, int32 EquipmentID)
{
	// 슬롯 수 보장
	while (SelectedEquipmentIDs.Num() <= SlotIndex)
	{
		SelectedEquipmentIDs.Add(-1);
	}
        
	SelectedEquipmentIDs[SlotIndex] = EquipmentID;
}

int32 ULRSaveGame::GetEquipmentSlot(int32 SlotIndex) const
{
	if (SelectedEquipmentIDs.IsValidIndex(SlotIndex))
	{
		return SelectedEquipmentIDs[SlotIndex];
	}
	return -1;
}

TArray<int32> ULRSaveGame::GetAllEquippedIDs() const
{
	return SelectedEquipmentIDs;
}
