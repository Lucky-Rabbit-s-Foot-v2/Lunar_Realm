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
// (260123) 김하신 제작. 제반 사항 구현.
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
	
	// 장비 슬롯 관리
	void SetEquipmentSlot(int32 SlotIndex, int32 EquipmentID);
	int32 GetEquipmentSlot(int32 SlotIndex) const;
	
	// 모든 장비 슬롯 가져오기
	TArray<int32> GetAllEquippedIDs() const;
	
	
	// ========================================
	// 소유 데이터(캐릭터/장비 인스턴스)
	// ========================================
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<int32, FPlayerCharacterInstance> OwnedCharacters;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<int32, FPlayerEquipmentInstance> OwnedEquipments;
    
	// ========================================
	// 선택된 로드아웃 정보
	// ========================================
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 SelectedCharacterID = -1;
    
	// 장비 슬롯을 배열로 변경 (3개 슬롯)
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TArray<int32> SelectedEquipmentIDs;
    
};
