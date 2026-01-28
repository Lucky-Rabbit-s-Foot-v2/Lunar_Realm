// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CollectionSubsystem.generated.h"

/**
 * 캐릭터/장비 도감 데이터 처리를 담당하는 중앙 집중식 서브시스템
 * 
 * 주요 기능:
 * - 플레이어 소유 캐릭터/장비 관리
 * - 레벨업 / 경험치 로직 관리 
 * - (추후) 인벤토리 데이터 관리
 */
//=============================================================================
// (260128) KHS 제작. 제반 사항 구현.
// =============================================================================


struct FPlayerCharacterInstance;
struct FPlayerEquipmentInstance;
class ULRSaveGame;

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterUpdated, int32, CharacterID, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentUpdated, int32, EquipmentID, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterUnlocked, int32, CharacterID, const FPlayerCharacterInstance&, CharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquipmentUnlocked, int32, EquipmentID, const FPlayerEquipmentInstance&, EquipmentData);


/**
 * 
 */
UCLASS()
class LUNAR_REALM_API UCollectionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//델리게이트
	UPROPERTY(BlueprintAssignable, Category = "LR|Collection")
	FOnCharacterUpdated OnCharacterUpdatedDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|Collection")
	FOnEquipmentUpdated OnEquipmentUpdatedDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|Collection")
	FOnCharacterUnlocked OnCharacterUnlockedDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|Collection")
	FOnEquipmentUnlocked OnEquipmentUnlockedDel;
	
private:
	//SaveGame로드 완료 시 호출 헬퍼
	UFUNCTION()
	void HandleSaveGameLoaded(ULRSaveGame* LoadedSave);
	
	//변경사항 SaveGame에 동기화 헬퍼
	void SyncToSaveGame();

public:
	//================================================================
	//캐릭터 관리 항목
	//================================================================
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Character")
	bool HasCharacter(int32 CharacterID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Character")
	FPlayerCharacterInstance GetCharacterInstance(int32 CharacterID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Character")
	void AddCharacter(int32 CharacterID, int32 StartLevel = 1);
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Character")
	void LevelUpCharacter(int32 CharacterID);
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Character")
	void AddCharacterExp(int32 CharacterID, int32 ExpAmount);
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Character")
	FORCEINLINE TMap<int32, FPlayerCharacterInstance> GetAllCharacters() const {return OwnedCharactersMap;}
	
	//================================================================
	//장비 관리 항목
	//================================================================
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Equipment")
	bool HasEquipment(int32 EquipmentID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Equipment")
	FPlayerEquipmentInstance GetEquipmentInstance(int32 EquipmentID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Equipment")
	void AddEquipment(int32 EquipmentID, int32 StartLevel = 1);
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Equipment")
	void LevelUpEquipment(int32 EquipmentID);
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Equipment")
	void AddEquipmentExp(int32 EquipmentID, int32 ExpAmount);
	
	
	UFUNCTION(BlueprintCallable, Category = "LR|Collection|Equipment")
	FORCEINLINE TMap<int32, FPlayerEquipmentInstance> GetAllEquipments() const {return OwnedEquipmentsMap;}
	
	
private:
	//도감 데이터
	UPROPERTY()
	TMap<int32, FPlayerCharacterInstance> OwnedCharactersMap;
	UPROPERTY()
	TMap<int32, FPlayerEquipmentInstance> OwnedEquipmentsMap;
	
};
