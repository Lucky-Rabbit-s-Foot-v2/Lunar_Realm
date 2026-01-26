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
// =============================================================================
UCLASS()
class LUNAR_REALM_API ULRSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	
public:
	// ========================================
	// 헬퍼 함수들
	// ========================================
	bool HasCharacter(int32 CharacterID) const;
	bool HasEquipment(int32 EquipmentID) const;
	
	// 캐릭터 슬롯 관리
	void SetCharacterPartySlot(int32 SlotIndex, int32 CharacterID);
	int32 GetCharacterID(int32 SlotIndex) const;
	
	//모든 캐릭터 슬롯 가져오기
	TArray<int32> GetAllCharacterSlots() const;
	
	//리더캐릭터 ID 반환.
	int32 GetLeaderCharacterID() const;
	
	// 리더캐릭터 장비 슬롯 관리
	void SetEquipmentSlot(int32 SlotIndex, int32 EquipmentID);
	int32 GetEquipmentID(int32 SlotIndex) const;
	
	// 리더캐릭터 모든 장비 슬롯 가져오기
	TArray<int32> GetAllEquippedIDs() const;
	
	
	// ========================================
	// 소유 도감 데이터(캐릭터/장비 인스턴스)
	// ========================================
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<int32, FPlayerCharacterInstance> OwnedCharacters;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<int32, FPlayerEquipmentInstance> OwnedEquipments;
    
	// ========================================
	// 선택된 로드아웃 정보
	// ========================================
	// 선택된 캐릭터 파티 정보
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<int32> SelectedCharactersIDs; // 4명[리더, 파티원1, 파티원2, 파티원3]
    
	// 선택된 리더 캐릭터 장비 정보
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<int32> SelectedEquipmentIDs; // 리더 장비 3개 [무기, 헬멧, 갑옷]
    
};
