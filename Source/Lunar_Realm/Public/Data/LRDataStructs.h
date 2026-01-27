// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h"
#include "LRDataStructs.generated.h"

// =============================================================================
/** 
 * FCharacterIDInfo, FEquipmentIDInfo 구성 요소
 * - 캐릭터/장비 ID 파싱 처리 결과 구조체
* 
// 캐릭터 ID (5자리)
// Format: T CC VV
// 10101 = 1 / 01 / 01
//         │   │    └─ Variant (등급/버전)
//         │   └────── Class (직업)
//         └────────── Type (캐릭터=1)

// 장비 ID (8자리)  
// Format: T CC III SS
// 20100102 = 2 / 01 / 001 / 02
//            │   │    │     └─ Set (세트효과)
//            │   │    └─────── Item (아이템 번호)
//            │   └──────────── Category (무기/방어구 등)
//            └──────────────── Type (장비=2)
 */
//=============================================================================
// (260126) KHS ID 도메인에 따라 ID파싱결과 처리 구조체 추가
// =============================================================================

//캐릭터 ID 파싱 결과를 처리하기 위한 구조체
USTRUCT(BlueprintType)
struct FCharacterIDInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	uint8 Type; //1 = 캐릭터(고정)
	
	UPROPERTY(BlueprintReadOnly)
	uint8 Class; //01 = 마법사 / 03 = 궁수
	
	UPROPERTY(BlueprintReadOnly)
	uint8 Variant; //01 = 기본
	
	FCharacterIDInfo() 
		: Type(0), Class(0), Variant(0)
	{}
};


//장비 ID 파싱 결과를 처리하기 위한 구조체
USTRUCT(BlueprintType)
struct FEquipmentIDInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	uint8 Type; //2 = 장비(고정)
	
	UPROPERTY(BlueprintReadOnly)
	uint8 Category; //01 = 근거리무기, 15 = 머리방어구
	
	UPROPERTY(BlueprintReadOnly)
	uint8 ItemNumber; //001 = 단검, 002 = 장검
	
	UPROPERTY(BlueprintReadOnly)
	uint8 SetID; //02 = 화염세트, 03 = 얼음세트
	
	FEquipmentIDInfo()
		: Type(0), Category(0), ItemNumber(0), SetID(0)
	{}
	
};


// =============================================================================
/** 
 * FCharacterStaticData 구성 요소
 * - 캐릭터 정적 데이터(도감 데이터 사용)
 * - ID, 이름, 캐릭터 설명, 이미지 텍스쳐
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
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
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<int32> SkillIDs;
};


// =============================================================================
/** 
 * FCharacterMultipliers 구성 요소
 * - 캐릭터 레벨별 스테이터스 승수 데이터
 * - 기본 체력/공격력/방어력 설정
 * - 새로운 스탯 요소 필요시 Row 추가 가능
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
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
// (260123) KHS 제작. 제반 사항 구현.
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
// (260123) KHS 제작. 제반 사항 구현.
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
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<int32> SkillIDs;
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
// (260123) KHS 제작. 제반 사항 구현.
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
 * FSetEffectData 구성 요소
 * - 장비세트효과 데이터
 * - 세트ID, 세트 이름, 필요 장착갯수, 스탯 퍼센트 보너스
 */
//=============================================================================
// (260126) KHS 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FSetEffectData : public FTableRowBase
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set")
	int32 SetID;  // 02 = 화염, 03 = 얼음
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set")
	FString SetName;  // "Fire Set", "Ice Set"
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set")
	int32 RequiredPieces = 3;  // 3개 풀세트
    
	// 스탯 퍼센트 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BonusHPPercent = 0.f;  // 10.0 = 10% 증가
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BonusAttackPercent = 0.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BonusDefensePercent = 0.f;
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
// (260123) KHS 제작. 제반 사항 구현.
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

// =============================================================================
/** 
 * FSkillStaticData 구성 요소
 * - 스킬 정적 데이터
 * - ID, 이름, 스킬 설명, 이미지 텍스쳐, 실제 사용 GA클래스
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
//=============================================================================
// (260126) KHS 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FSkillStaticData : public FTableRowBase
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillID;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SkillName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> SkillIcon;
    
	// 실제 GA 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
};

