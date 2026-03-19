// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGame/LRSaveGame.h"
#include "Data/LRDataStructs.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

/**
 * SaveGame 파일의 로드/저장을 담당하는 중앙 집중식 서브시스템
 * 
 * 주요 기능:
 * - 파일 I/O 
 * - 현재 선택된 캐릭터/장비 정보 관리
 */
//=============================================================================
// (260126) KHS 제작. 제반 사항 구현.
// (260211) PYI 재화 접근 인터페이스 추가.
// =============================================================================

//SaveGame 저장/로드 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, ULRSaveGame*, LoadedSave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameSaved);


UCLASS()
class LUNAR_REALM_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	//델리게이트
	UPROPERTY(BlueprintAssignable, Category = "LR|SaveGame")
	FOnSaveGameLoaded OnSaveGameLoadedDel;
	
	UPROPERTY(BlueprintAssignable, Category = "LR|SaveGame")
	FOnSaveGameSaved OnSaveGameSavedDel;
	
private:
	//내부 헬퍼
	void CreateNewSaveGame();
	
public:
	//핵심 
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void LoadGame();
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void SaveGame();
	
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	ULRSaveGame* GetCurrentSaveGame() const {return CurrentSaveGame;}

	//======================================
	//세이브 캐릭터/장비 정보 관리
	//======================================
	//선택된 캐릭터 파티 정보 관리
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void SetPartySlot(int32 Slots, FName CharacterID);
	
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	FName GetPartyCharacterID(int32 Slot) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	TArray<FName> GetAllPartyCharactersIDs() const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	FName GetLeaderCharacterID() const;
	
	//선택된 리더 캐릭터 장비 정보 관리
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void SetLeaderEquipmentSlot(int32 Slots, FGuid EquipmentIDs);
	
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	FGuid GetLeaderEquipmentID(int32 SlotIdx) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	TArray<FGuid> GetAllLeaderEquipmentIDs() const;
	
	//======================================
	// 세이브 재화 정보 관리 (260211 PYI추가)
	//======================================
	// 재화 접근 인터페이스
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame|Currency")
	int32 GetCurrency(ELRCurrencyType Type) const;

	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame|Currency")
	void AddCurrency(ELRCurrencyType Type, int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame|Currency")
	bool TrySpendCurrency(ELRCurrencyType Type, int32 Cost);

	// 저장 
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void UpdateLastSavedTimeAndSave();
	
	//======================================
	// 스테이지 정보 관리 (260319 PJB 추가)
	//======================================
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame|Stage")
	FStageClearedData GetStageClearedData(FName InID) const;

	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame|Stage")
	void SaveStageClearedData(FStageClearedData& InData);

private:
	UPROPERTY()
	TObjectPtr<ULRSaveGame> CurrentSaveGame;
	
	const FString SaveSlotName = TEXT("PlayerSaveSlot");
	const uint32 UserIndex = 0;

};
