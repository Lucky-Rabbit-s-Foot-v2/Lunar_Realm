// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/LRDataStructs.h"

#include "GameFramework/SaveGame.h"

#include "LRSaveGame.generated.h"

// =============================================================================
/** 
 * ULRSaveGame 구성 요소
 * - 직렬화 구현을 위해 런타임 인스턴스 정보 보관
 * - SaveGameSubsys / InventorySubsys를 통해 역직렬화 후 로드 데이터 세팅
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260211) PYI 제작. 재화, 가챠저장용 데이터(천장카운터, 트랜잭션), 유저 재화 기본값
// =============================================================================
UCLASS()
class LUNAR_REALM_API ULRSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	
public:
	// ========================================
	// 로드아웃 헬퍼 함수들
	// ========================================
	bool HasCharacter(FName CharacterID) const;
	bool HasEquipment(FName EquipmentID) const;
	
	// 캐릭터 슬롯 관리
	void SetCharacterPartySlot(int32 SlotIndex, FName CharacterID);
	FName GetCharacterID(int32 SlotIndex) const;
	
	//모든 캐릭터 슬롯 가져오기
	TArray<FName> GetAllCharacterSlots() const;
	
	//리더캐릭터 ID 반환.
	FName GetLeaderCharacterID() const;
	
	// 리더캐릭터 장비 슬롯 관리
	void SetEquipmentSlot(int32 SlotIndex, FName EquipmentID);
	FName GetEquipmentID(int32 SlotIndex) const;
	
	// 리더캐릭터 모든 장비 슬롯 가져오기
	TArray<FName> GetAllEquippedIDs() const;
	
	// ========================================
	// 소유 도감 데이터(캐릭터/장비 인스턴스)
	// ========================================
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FName, FPlayerCharacterInstance> OwnedCharacters;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FName, FPlayerEquipmentInstance> OwnedEquipments;
    
	// ========================================
	// 선택된 로드아웃 정보
	// ========================================
	// 선택된 캐릭터 파티 정보
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<FName> SelectedCharactersIDs; // 4명[리더, 파티원1, 파티원2, 파티원3]
    
	// 선택된 리더 캐릭터 장비 정보
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<FName> SelectedEquipmentIDs; // 리더 장비 3개 [무기, 헬멧, 갑옷]
    
	// ========================================
	// Currency
	// ========================================
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 Gold = 0;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CrescentTicket = 0;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 FullMoonTicket = 0;

	// 마지막 저장(UTC) - "언제 SaveGameToSlot 되었는지"
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FDateTime LastUpdatedUtc;

	// ========================================
	// Gacha (Pity / Pending Transactions)
	// ========================================
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FName, int32> GachaPityCounterMap;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FGuid, FLRGachaPendingTransaction> GachaPendingTransactions;

	// 신규 유저 기본값 세팅(테스트/첫 실행용)
	UFUNCTION(BlueprintCallable, Category = "LR|SaveGame")
	void InitializeNewPlayerDefaults()
	{
		Gold = 1000;
		CrescentTicket = 1000;
		FullMoonTicket = 1000;
		LastUpdatedUtc = FDateTime::UtcNow();
	}
};
