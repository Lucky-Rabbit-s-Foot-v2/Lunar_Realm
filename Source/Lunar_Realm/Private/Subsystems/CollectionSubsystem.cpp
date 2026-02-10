 // Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/CollectionSubsystem.h"
#include "Data/LRDataStructs.h"
#include "Engine/GameInstance.h"
#include "SaveGame/LRSaveGame.h"
#include "Subsystems/SaveGameSubsystem.h"

 void UCollectionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
 {

 	Collection.InitializeDependency<USaveGameSubsystem>();
 	
	Super::Initialize(Collection);
 	
 	//SaveGameSubsystem의 로드완료 이벤트 구독
 	USaveGameSubsystem* saveGameSys = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
 	check(saveGameSys);
 	saveGameSys->OnSaveGameLoadedDel.AddDynamic(this, &UCollectionSubsystem::HandleSaveGameLoaded);
 	LR_INFO(TEXT("Collection Subsystem Initialized"));
 }

 void UCollectionSubsystem::HandleSaveGameLoaded(ULRSaveGame* LoadedSave)
 {
 	if (!ensureMsgf(LoadedSave, TEXT("Invalid LoadedSave Instance")))
 	{
 		return;
 	}
 	
 	//SaveGame 데이터를 내부 맵에 복사
 	OwnedCharactersMap = LoadedSave->OwnedCharacters;
 	OwnedEquipmentsMap = LoadedSave->OwnedEquipments;
 	
 	LR_INFO(TEXT("Collection loaded : %d characters, %d equipments"), OwnedCharactersMap.Num(), OwnedEquipmentsMap.Num());
 }

 void UCollectionSubsystem::SyncToSaveGame()
 {
 	USaveGameSubsystem* saveGameSys = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
 	check(saveGameSys);
 	
 	ULRSaveGame* saveGame = saveGameSys->GetCurrentSaveGame();
 	if (!ensureMsgf(saveGame, TEXT("Invalid Current SaveGame Instance")))
 	{
 		return;
 	}
 	
 	//변경사항을 SaveGame에 동기화
 	saveGame->OwnedCharacters = OwnedCharactersMap;
 	saveGame->OwnedEquipments = OwnedEquipmentsMap;
 	
 	//자동 저장
 	saveGameSys->SaveGame();
 }

 bool UCollectionSubsystem::HasCharacter(FName CharacterID) const
 {
 	return OwnedCharactersMap.Contains(CharacterID) && OwnedCharactersMap[CharacterID].bIsUnlocked;
 }

 FPlayerCharacterInstance UCollectionSubsystem::GetCharacterInstance(FName CharacterID) const
 {
 	if (OwnedCharactersMap.Contains(CharacterID))
 	{
 		return OwnedCharactersMap[CharacterID];
 	}
 	
 	//없으면 빈 인스턴스 생성
 	return FPlayerCharacterInstance();
 }

 void UCollectionSubsystem::AddCharacter(FName CharacterID, int32 StartLevel)
 {
 	if (!OwnedCharactersMap.Contains(CharacterID))
 	{
 		FPlayerCharacterInstance newCharacter(CharacterID, StartLevel);
 		OwnedCharactersMap.Add(CharacterID, newCharacter);
 		
 		//캐릭터 해금 이벤트 브로드캐스트
 		OnCharacterUnlockedDel.Broadcast(CharacterID, newCharacter);
 		//업데이트 정보 동기화
 		SyncToSaveGame();
 		
 		LR_INFO(TEXT("Character ID : %s - unlocked at level %d"), *CharacterID.ToString(), StartLevel);
 	}
    else
    {
	    LR_WARN(TEXT("Character ID already in use"));
    }
 }

 void UCollectionSubsystem::LevelUpCharacter(FName CharacterID)
 {
 	FPlayerCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);
 	if (!ensureMsgf(instance, TEXT("Invalid Character ID")))
 	{
 		return;
 	}
 	
 	//TODO FPlayerCharacterInstance/EquipmentInstance를 불변객체화 할 필요가 있음. 
 	//레벨업 로직
 	instance->CurrentLevel++;
 	instance->CurrentExp = 0;
 	
 	OnCharacterUpdatedDel.Broadcast(CharacterID, instance->CurrentLevel);
 	SyncToSaveGame();
 	
 	LR_INFO(TEXT("Character %s leveled up to %d"), *CharacterID.ToString(), instance->CurrentLevel);
 }

 void UCollectionSubsystem::AddCharacterExp(FName CharacterID, int32 ExpAmount)
 {
 	FPlayerCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);
 	if (!ensureMsgf(instance, TEXT("Invalid Character ID")))
 	{
 		return;
 	}
 	
 	instance->CurrentExp += ExpAmount;
 	
 	//레벨업 체크
 	//TODO 레벨 구간 경험치 요구량 테이블 연결 예정
 	const int32 EXPforLevelup = 500;
 	if (instance->CurrentExp >= EXPforLevelup)
 	{
 		LevelUpCharacter(CharacterID);
 	}
    else
    {
	    SyncToSaveGame();
    }
 }

 bool UCollectionSubsystem::HasEquipment(FName EquipmentID) const
 {
 	return OwnedEquipmentsMap.Contains(EquipmentID) && OwnedEquipmentsMap[EquipmentID].bIsUnlocked;
 }

 FPlayerEquipmentInstance UCollectionSubsystem::GetEquipmentInstance(FName EquipmentID) const
 {
 	if (OwnedEquipmentsMap.Contains(EquipmentID))
 	{
 		return OwnedEquipmentsMap[EquipmentID];
 	}
 	
 	return FPlayerEquipmentInstance();
 }

 void UCollectionSubsystem::AddEquipment(FName EquipmentID, int32 StartLevel)
 {
 	if (!OwnedEquipmentsMap.Contains(EquipmentID))
 	{
 		FPlayerEquipmentInstance newEquipment(EquipmentID, StartLevel);
 		OwnedEquipmentsMap.Add(EquipmentID, newEquipment);
 		
 		//해금 이벤트 브로드캐스트
 		OnEquipmentUnlockedDel.Broadcast(EquipmentID, newEquipment);
 		SyncToSaveGame();
 		
 		LR_INFO(TEXT("Equipment %s unlocked at level %d"), *EquipmentID.ToString(), StartLevel);
 	}
    else
    {
	    LR_WARN(TEXT("Equipment ID already in use"));
    }
 }

 void UCollectionSubsystem::LevelUpEquipment(FName EquipmentID)
 {
 	FPlayerEquipmentInstance* instance = OwnedEquipmentsMap.Find(EquipmentID);
 	if (!ensureMsgf(instance, TEXT("Invalid EquipmentID")))
 	{
 		return;
 	}
 	
 	instance->CurrentLevel++;
 	//장비 레벨업 이벤트 브로드캐스트
 	OnEquipmentUpdatedDel.Broadcast(EquipmentID, instance->CurrentLevel);
 	SyncToSaveGame();
 	
 	LR_INFO(TEXT("Equipment %s leveled up to %d"), *EquipmentID.ToString(), instance->CurrentLevel);
 }

 void UCollectionSubsystem::AddEquipmentExp(FName EquipmentID, int32 ExpAmount)
 {
 	
 	FPlayerEquipmentInstance* instance = OwnedEquipmentsMap.Find(EquipmentID);
 	if (!ensureMsgf(instance, TEXT("Invalid EquipmentID")))
 	{
 		return;
 	}
 	
 	instance->CurrentExp += ExpAmount;
 	
 	//레벨업 체크
 	//TODO 레벨 구간 경험치 요구량 테이블 연결 예정
 	const int32 EXPforLevelup = 500;
 	if (instance->CurrentExp >= EXPforLevelup)
 	{
 		LevelUpEquipment(EquipmentID);
 	}
 	else
 	{
 		SyncToSaveGame();
 	}
 }
