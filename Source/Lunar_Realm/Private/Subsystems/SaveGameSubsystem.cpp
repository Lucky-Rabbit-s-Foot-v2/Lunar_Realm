// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"

void USaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LR_INFO(TEXT("SaveGameSubsystem Initialized"));
	LoadGame();
}

void USaveGameSubsystem::Deinitialize()
{
	SaveGame();
	Super::Deinitialize();
}

void USaveGameSubsystem::CreateNewSaveGame()
{
	//세이브게임 로드가 안될 시 사용하는 더미 데이터
	CurrentSaveGame = Cast<ULRSaveGame>(UGameplayStatics::CreateSaveGameObject(ULRSaveGame::StaticClass()));
	
#if WITH_EDITOR
	//테스트용 데이터......
	//도감 보유 캐릭터 정보
	CurrentSaveGame->OwnedCharacters.Add(10102, FPlayerCharacterInstance(10102, 10)); //리더
	CurrentSaveGame->OwnedCharacters.Add(10301, FPlayerCharacterInstance(10301, 3)); //파티원 1
	CurrentSaveGame->OwnedCharacters.Add(10201, FPlayerCharacterInstance(10201, 4)); //파티원 2
	CurrentSaveGame->OwnedCharacters.Add(10101, FPlayerCharacterInstance(10101, 4)); //파티원 3
	
	//도감 보유 장비 정보
	CurrentSaveGame->OwnedEquipments.Add(20100201, FPlayerEquipmentInstance(20100201, 3)); //리더장비 무기
	CurrentSaveGame->OwnedEquipments.Add(21150102, FPlayerEquipmentInstance(21150102, 3)); //리더장비 헬멧
	CurrentSaveGame->OwnedEquipments.Add(21255101, FPlayerEquipmentInstance(21255101, 3)); //리더장비 갑옷
	
	//선정 파티 정보
	CurrentSaveGame->SelectedCharactersIDs.Add(10102);
	CurrentSaveGame->SelectedCharactersIDs.Add(10301);
	CurrentSaveGame->SelectedCharactersIDs.Add(10201);
	CurrentSaveGame->SelectedCharactersIDs.Add(10301);
	
	//리더 장비 정보
	CurrentSaveGame->SelectedEquipmentIDs.Add(20100201);
	CurrentSaveGame->SelectedEquipmentIDs.Add(21150102);
	CurrentSaveGame->SelectedEquipmentIDs.Add(21255102);
	
#endif
	
	OnSaveGameLoaded.Broadcast(CurrentSaveGame);
}

void USaveGameSubsystem::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		CurrentSaveGame = Cast<ULRSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
		if (!ensureMsgf(CurrentSaveGame, TEXT("Could not load GameSaveSlot")))
		{
			return;
		}
		
		//브로드캐스트
		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
	}
	else
	{
		LR_INFO(TEXT("No Save Game found, creating new one"));
		CreateNewSaveGame();
	}
}

void USaveGameSubsystem::SaveGame()
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CANNOT SAVE : CurrentSaveGame is NULL")))
	{
		return;
	}
	
	if (UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SaveSlotName, UserIndex))
	{
		LR_INFO(TEXT("Save Game Successfully"));
		OnSaveGameSaved.Broadcast();
	}
}

void USaveGameSubsystem::SetPartySlot(int32 Slots, int32 CharacterID)
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return;
	}
	
	CurrentSaveGame->SetCharacterPartySlot(Slots, CharacterID);
	SaveGame(); //자동저장
	
	LR_INFO(TEXT("Character Slot %d set to ID %d"), Slots, CharacterID);
}

int32 USaveGameSubsystem::GetPartyCharacterID(int32 Slot) const
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return -1;
	}
	
	return CurrentSaveGame->GetCharacterID(Slot);
}

TArray<int32> USaveGameSubsystem::GetAllPartyCharactersIDs() const
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return TArray<int32>();
	}
	
	return CurrentSaveGame->GetAllCharacterSlots();
}

int32 USaveGameSubsystem::GetLeaderCharacterID() const
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return -1;
	}
	
	return CurrentSaveGame->GetLeaderCharacterID();
}

void USaveGameSubsystem::SetLeaderEquipmentSlot(int32 Slots, int32 EquipmentIDs)
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return;
	}
	
	CurrentSaveGame->SetEquipmentSlot(Slots, EquipmentIDs);
	SaveGame(); //자동저장
	
	LR_INFO(TEXT("Character Slot %d set to ID %d"), Slots, EquipmentIDs);
}

int32 USaveGameSubsystem::GetLeaderEquipmentID(int32 SlotIdx) const
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return -1;
	}
	
	return CurrentSaveGame->GetEquipmentID(SlotIdx);
}

TArray<int32> USaveGameSubsystem::GetAllLeaderEquipmentIDs() const
{
	if (!ensureMsgf(CurrentSaveGame, TEXT("CurrentSaveGame is NULL")))
	{
		return TArray<int32>();
	}
	
	return CurrentSaveGame->GetAllEquippedIDs();
}
