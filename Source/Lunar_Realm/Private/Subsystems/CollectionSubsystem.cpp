// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "SaveGame/LRSaveGame.h"

#include "Algo/Count.h"
#include "Data/LRDataStructs.h"
#include "Engine/GameInstance.h"

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

	//GameData에서 전체 캐릭터/장비데이터 가져오기
	UGameDataSubsystem* GameDataSys = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	check(GameDataSys);

	//캐릭터 도감 초기화
	TArray<FName> AllCharacterKeys = GameDataSys->GetAllCharacterIDs();
	OwnedCharactersMap.Empty();
	for (FName Key : AllCharacterKeys)
	{
		FCharacterInstance instance;

		instance.CharacterID = Key;
		instance.bIsUnlocked = false;
		instance.CurrentLevel = 1;
		instance.CurrentExp = 0;

		OwnedCharactersMap.Add(Key, instance);
	}

	//SaveGame 데이터로 덮어 쓰기
	TMap<FName, FCharacterInstance> savedCharacterList = LoadedSave->GetOwnedCharactersList();
	for (auto& pair : savedCharacterList)
	{
		FName key = pair.Key;
		if (OwnedCharactersMap.Contains(key))
		{
			OwnedCharactersMap[key] = pair.Value;
		}
		else
		{
			LR_WARN(TEXT("Savegame has unknown character : %s"), *key.ToString());
		}
	}

	OwnedEquipmentsArray = LoadedSave->GetOwnedEquipmentsList();

	LR_INFO(TEXT("Collection loaded: %d/%d characters unlocked, %d equipment instances"),
		LoadedSave->GetOwnedCharactersList().Num(), AllCharacterKeys.Num(),
		OwnedEquipmentsArray.Num());
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
	saveGame->SetOwnedCharactersList(OwnedCharactersMap);
	saveGame->SetOwnedEquipmentsList(OwnedEquipmentsArray);

	//자동 저장
	saveGameSys->SaveGame();
}

bool UCollectionSubsystem::HasCharacter(FName CharacterID) const
{
	const FCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);
	return instance && instance->bIsUnlocked;
}

FCharacterInstance UCollectionSubsystem::GetCharacterInstance(FName CharacterID) const
{
	const FCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);

	if (instance)
	{
		return *instance;
	}

	//없으면 빈 인스턴스 생성
	LR_WARN(TEXT("Character %s not found in collection"), *CharacterID.ToString());
	return FCharacterInstance();
}

void UCollectionSubsystem::AddCharacter(FName CharacterID, int32 StartLevel)
{
	if (!OwnedCharactersMap.Contains(CharacterID))
	{
		LR_ERROR(TEXT("Character %s not found in Collection! Check GameData"), *CharacterID.ToString());
		return;
	}

	FCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);
	//이미 해금된 경우
	if (instance->bIsUnlocked)
	{
		LR_WARN(TEXT("Character %s already unlocked"), *CharacterID.ToString());
		return;
	}

	//미해금만 해금 처리
	instance->bIsUnlocked = true;
	instance->CurrentLevel = StartLevel;
	instance->CurrentExp = 0;
	instance->AcquisitionTime = FDateTime::UtcNow();

	//캐릭터 해금 이벤트 브로드캐스트
	OnCharacterUnlockedDel.Broadcast(CharacterID, *instance);
	//업데이트 정보 동기화
	SyncToSaveGame();

	LR_INFO(TEXT("Character ID : %s - unlocked at level %d"), *CharacterID.ToString(), StartLevel);
}

void UCollectionSubsystem::LevelUpCharacter(FName CharacterID)
{
	FCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);
	if (!ensureMsgf(instance, TEXT("Invalid Character ID")))
	{
		return;
	}

	if (!instance->bIsUnlocked)
	{
		LR_ERROR(TEXT("Cannot level up unlocked character : %s "), *CharacterID.ToString());
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
	FCharacterInstance* instance = OwnedCharactersMap.Find(CharacterID);
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


TArray<FName> UCollectionSubsystem::GetAllCharacterIDs() const
{
	TArray<FName> IDs;
	OwnedCharactersMap.GetKeys(IDs);
	return IDs;
}


TArray<FName> UCollectionSubsystem::GetUnlockedCharacterIDs() const
{
	TArray<FName> IDs;
	Algo::TransformIf(
		OwnedCharactersMap,
		IDs, //대상
		[](const auto& pair) {return pair.Value.bIsUnlocked; }, //조건
		[](const auto& pair) {return pair.Key; } //변환
	);

	return IDs;
}

bool UCollectionSubsystem::HasEquipment(FName EquipmentID) const
{
	return OwnedEquipmentsArray.ContainsByPredicate(
		[EquipmentID](const auto& instance)
		{
			return instance.EquipmentID == EquipmentID;
		});
}

FEquipmentInstance UCollectionSubsystem::GetEquipmentInstance(FGuid InstanceID) const
{

	const FEquipmentInstance* found = Algo::FindByPredicate(
		OwnedEquipmentsArray,
		[InstanceID](const FEquipmentInstance& instance)
		{
			return instance.InstanceID == InstanceID;
		});

	if (found)
	{
		return *found;
	}

	LR_WARN(TEXT("Equipment ID %s not found"), *InstanceID.ToString());
	return FEquipmentInstance();
}

TArray<FEquipmentInstance> UCollectionSubsystem::GetEquipmentInstancesByKey(FName EquipmentID) const
{
	return OwnedEquipmentsArray.FilterByPredicate(
		[EquipmentID](const FEquipmentInstance& instance)
		{
			return instance.EquipmentID == EquipmentID;
		});
}

FGuid UCollectionSubsystem::AddEquipment(FName EquipmentID, int32 StartLevel)
{
	//GameData에 존재하는지 검증
	UGameDataSubsystem* GameDataSys = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	check(GameDataSys);

	if (!GameDataSys->GetAllEquipmentIDs().Contains(EquipmentID))
	{
		LR_ERROR(TEXT("Equipment %s not found in GameData"), *EquipmentID.ToString());
		return FGuid();
	}

	// 새인스턴스 생성
	FEquipmentInstance newEquipment(EquipmentID, StartLevel);
	OwnedEquipmentsArray.Add(newEquipment);

	//해금 이벤트 브로드캐스트
	OnEquipmentUnlockedDel.Broadcast(EquipmentID, newEquipment);

	SyncToSaveGame();
	LR_INFO(TEXT("Equipment %s acquired (InstanceID: %s, Level: %d)"),
		*EquipmentID.ToString(),
		*newEquipment.InstanceID.ToString(),
		StartLevel);

	return newEquipment.InstanceID;
}


void UCollectionSubsystem::LevelUpEquipment(FGuid InstanceID)
{
	FEquipmentInstance* instance = Algo::FindByPredicate(OwnedEquipmentsArray,
		[InstanceID](const FEquipmentInstance& instance)
		{
			return instance.InstanceID == InstanceID;
		});

	if (!ensureMsgf(instance, TEXT("Equipment InstanceID %s not found for level up"), *InstanceID.ToString()))
	{
		return;
	}

	instance->CurrentLevel++;
	instance->CurrentExp = 0;

	OnEquipmentUpdatedDel.Broadcast(InstanceID, instance->CurrentLevel);
	SyncToSaveGame();

	LR_INFO(TEXT("Equipment %s (ID: %s) leveled up to %d"),
		*instance->EquipmentID.ToString(),
		*InstanceID.ToString(),
		instance->CurrentLevel);

}

void UCollectionSubsystem::AddEquipmentExp(FGuid EquipmentID, int32 ExpAmount)
{
	FEquipmentInstance* instance = Algo::FindByPredicate(OwnedEquipmentsArray,
		[EquipmentID](const FEquipmentInstance& instance)
		{
			return instance.InstanceID == EquipmentID;
		});


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

int32 UCollectionSubsystem::GetEquipmentCounts(FName EquipmentID) const
{
	return Algo::CountIf(OwnedEquipmentsArray,
		[EquipmentID](const FEquipmentInstance& instance)
		{
			return instance.EquipmentID == EquipmentID;
		});
}

TArray<FEquipmentInstance> UCollectionSubsystem::GetAllEquipments() const
{
	return OwnedEquipmentsArray;
}
