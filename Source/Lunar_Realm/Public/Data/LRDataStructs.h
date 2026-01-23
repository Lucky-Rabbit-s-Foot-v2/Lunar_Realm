// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h"
#include "LRDataStructs.generated.h"

// =============================================================================
/** 
 * FCharacterStaticData 구성 요소
 * - 캐릭터 정적 데이터(도감 데이터 사용)
 * - ID, 이름, 캐릭터 설명, 이미지 텍스쳐
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
//=============================================================================
// (260123) 김하신 제작. 제반 사항 구현.
// =============================================================================

USTRUCT(BlueprintType)
struct FCharacterStaticData : public FTableRowBase
{
	GENERATED_BODY()
    
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	int32 CharacterID;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FString CharacterName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText Description;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> CharacterTexture;
    
};


// =============================================================================
/** 
 * FCharacterMultipliers 구성 요소
 * - 캐릭터 레벨별 스테이터스 승수 데이터
 * - 기본 체력/공격력/방어력 설정
 * - 새로운 스탯 요소 필요시 Row 추가 가능
 */
//=============================================================================
// (260123) 김하신 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FCharacterMultipliers : public FTableRowBase
{
	GENERATED_BODY()
    
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Multiplier")
	float HPMultiplier;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Multiplier")
	float AttackMultiplier;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Multiplier")
	float DefenseMultiplier;
};


// =============================================================================
/** 
 * FPlayerCharacterInstance 구성 요소
 * - 런타임중 사용하는 실제 인스턴스 공유 데이터
 * - SaveGame 로드 -> InventorySubsys -> 실제 런타임 인스턴스
 * - 아웃게임 UI(도감, 파티 편성), 인게임 PlayerState (레벨 정보) 등에서 사용
 * - 캐릭터 ID, 캐릭터레벨, 캐릭터 경험치, 캐릭터 해금상태
 */
//=============================================================================
// (260123) 김하신 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FPlayerCharacterInstance
{
	GENERATED_BODY()
    
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CharacterID;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentLevel = 1;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentExp = 0;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked = false;
    
	FPlayerCharacterInstance() {}
    
	FPlayerCharacterInstance(int32 InID, int32 InLevel = 1)
		: CharacterID(InID), CurrentLevel(InLevel), CurrentExp(0), bIsUnlocked(true)
	{}
};




// =============================================================================
/** 
 * FEquipmentStaticData 구성 요소
 * - 장비별 정적 데이터(도감용)
 * - 장비ID, 이름, 설명, 2D텍스쳐, 장비별 GameAbility(GAS)
 * - TSoftObjectPtr로 비동기 로딩.
 * - 연동된 게임 어빌리티 정보를 통해 장비스킬 가능하도록 연결(DataSubsys)
 */
//=============================================================================
// (260123) 김하신 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FEquipmentStaticData : public FTableRowBase
{
	GENERATED_BODY()
    
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	int32 EquipmentID;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FString EquipmentName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText Description;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> EquipmentTexture;
    
	// GA는 그대로 유지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
};


// =============================================================================
/** 
 * FEquipmentBonus 구성 요소
 * - 장비별 스탯 보너스 데이터
 * - 장비ID, 각 스테이터스별 보너스 스탯 정보, 레벨별 보너스 스탯 가산 정보.
 * - TSoftObjectPtr로 비동기 로딩.
 * - TODO 이후 CT형태로 변경 
 */
//=============================================================================
// (260123) 김하신 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FEquipmentBonus : public FTableRowBase
{
	GENERATED_BODY()
    
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	int32 EquipmentID;
    
	// 레벨별 스탯 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float BaseHPBonus = 100.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float HPBonusPerLevel = 50.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float BaseAttackBonus = 20.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float AttackBonusPerLevel = 10.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float BaseDefenseBonus = 10.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float DefenseBonusPerLevel = 5.f;
    
	// 또는 CurveTable 사용 (선택)
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	// TSoftObjectPtr<UCurveTable> StatBonusCurve;
};



// =============================================================================
/** 
 * FPlayerEquipmentInstance 구성 요소
 * - 런타임중 사용하는 실제 인스턴스 공유 데이터
 * - SaveGame 로드 -> InventorySubsys -> 실제 런타임 인스턴스
 * - 아웃게임 UI(도감, 장비 장착), 인게임 PlayerState (장비 정보) 등에서 사용
 * - 장비 ID, 장비 레벨, 장비 해금상태
 */
//=============================================================================
// (260123) 김하신 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FPlayerEquipmentInstance
{
	GENERATED_BODY()
    
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 EquipmentID;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentLevel = 1;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked = false;
    
	FPlayerEquipmentInstance() {}
    
	FPlayerEquipmentInstance(int32 InID, int32 InLevel = 1)
		: EquipmentID(InID), CurrentLevel(InLevel), bIsUnlocked(true)
	{}
	
	
};




