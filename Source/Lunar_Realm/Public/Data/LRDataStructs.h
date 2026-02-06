// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShaderCompilerJobTypes.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataTable.h"
#include "Data/LREnumType.h"
#include "Elements/Framework/TypedElementQueryBuilder.h"
#include "LRDataStructs.generated.h"



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
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	int32 CharacterID;
	
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
	int32 CurrentLevel;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentExp;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FDateTime AcquisitionTime; //획득 타임스탬프
	
	FPlayerCharacterInstance() 
		: CharacterID(0), CurrentLevel(1), CurrentExp(0), bIsUnlocked(false), AcquisitionTime(FDateTime::MinValue())
	{	}
    
	FPlayerCharacterInstance(int32 InID, int32 InLevel = 1)
		: CharacterID(InID), CurrentLevel(InLevel), CurrentExp(0), bIsUnlocked(true), AcquisitionTime(FDateTime::Now())
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
	int32 CurrentLevel;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentExp;
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FDateTime AcquisitionTime;
	
	FPlayerEquipmentInstance() 
		: EquipmentID(0), CurrentLevel(1), CurrentExp(0), bIsUnlocked(false), AcquisitionTime(FDateTime::MinValue())
	{}
    
	FPlayerEquipmentInstance(int32 InID, int32 InLevel = 1)
		: EquipmentID(InID), CurrentLevel(InLevel), CurrentExp(0), bIsUnlocked(true), AcquisitionTime(FDateTime::Now())
	{}
	
	
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


// =============================================================================
/**
 * FEnemyStaticData 구성 요소
 * - 캐릭터 정적 데이터(인게임 스폰 데이터)
 * - ID, 이름, 적 설명(TBD), 이동 속도, 공격력, 체력, 이미지 텍스쳐(TBD - 보스), 실제 사용 GA클래스(TBD - 노말, 엘리트 - 1개 /보스 - 3개)
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
 //=============================================================================
 // (260204) KWB 제작. 제반 사항 구현.
 // =============================================================================
USTRUCT(BlueprintType)
struct FEnemyStaticData : public FTableRowBase
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	int32 CharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	int32 Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	int32 Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> CharacterTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
};





// =============================================================================
/** 
 * FCharacterIDInfo, FEquipmentIDInfo 구성 요소
 * - 캐릭터/장비 ID 파싱 처리 결과 구조체
* 
// 캐릭터 ID (8자리)
// Format: D G TT CC VV
// 11010101 = 1 / 1 / 01 / 01 / 01
//           │   │    │    │    └─ Variant (버전)
//           │   │    │    └────── Class (직업)
//           │   │    └────────── AttackType(공격스타일 - 원/근)
//           │   └─────────────── Grade(일반/엘리트/유니크/에픽/전설)
//           └────────────────────  Domain (캐릭터=1)

// 적 캐릭터 ID (8자리)
// Format: D G TT CC VV
// 31010101 = 3 / 1 / 01 / 01 / 01
//            │   │    │    │    └─ Variant (넘버링)
//            │   │    │    └────── Class (슬라임/고블린/골렘)
//            │   │    └─────────── AttackType (근거리/원거리)
//            │   └─────────────── Grade(일반/엘리트/유니크/에픽/전설)
//            └─────────────── Domain (Enemy = 3)

// 장비 ID (8자리)  
// Format: D G C SS VVV
// 20100102 = 2 / 1 / 1 / 02 / 001
//            │   │   │   │     └─ Variant(아이템 번호)
//            │   │   │   └─────── Set (세트효과)
//            │   │   └──────────── Category (무기/방어구 등)
//            │   └─────────────── Grade(일반/엘리트/유니크/에픽/전설)
//            └──────────────── Domain (장비=2)

// 스킬 ID (8자리) - 현재 따로 구성이 없어 도메인만 구분.
// Format: D X X XX VVV
// 40000001 = 4 / 0 / 0 / 00 / 001
//            │   │   │   │     └─ Variant(아이템 번호)
//            │   │   │   └─────── 미사용
//            │   │   └──────────── 미사용
//            │   └─────────────── 미사용
//            └──────────────── Domain (스킬 = 4)
 */
//=============================================================================
// (260126) KHS ID 도메인에 따라 ID파싱결과 처리 구조체 추가
// (260204) KWB 적 캐릭터용 ID 도메인 정보, ID파싱결과 처리 구조체 추가
// (260205) KHS ID INFO 반환방식 통합.(ID 7자리로 통합)
// =============================================================================

//ID 파싱 공통 사용 구조체(캐릭터, 장비, 적, 스킬)
USTRUCT(BlueprintType)
struct FEntityIDInfo
{
	GENERATED_BODY()

public:
	// 1. 원본 데이터 (ID 보존)
	UPROPERTY(BlueprintReadOnly)
	int32 FullID;

	// 2. 기본 도메인 정보 (언제나 파싱됨)
	UPROPERTY(BlueprintReadOnly)
	ELRDomain Domain;

	// 3. 생성자: ID를 넣으면 즉시 기본 도메인만 파싱 (기본은 캐릭터)
	FEntityIDInfo() 
	: FullID(0), Domain(ELRDomain::CHARACTER) 
	{}
	
	FEntityIDInfo(int32 InID) 
	: FullID(InID)
	{
		Domain = static_cast<ELRDomain>(InID / 10000000); //1번 자리
	}

	//==========================
	// --- 헬퍼 함수 (Getter) ---
	//==========================

	//[공통] 등급 반환 (2번자리)
	ELRGrade GetGrade() const { return static_cast<ELRGrade>((FullID / 1000000) % 10 );}

	//[공통] 상세 버전 반환(필요시.)
	int32 GetVariantType() const
	{
		if (Domain == ELRDomain::CHARACTER || Domain == ELRDomain::ENEMY) return FullID % 100; //7-8번
		else if (Domain == ELRDomain::EQUIPMENT || Domain == ELRDomain::SKILL) return FullID % 1000; //6-8번
		else return -1;
	}
	
	//[캐릭터/적] 공격타입 반환(3-4번)
	ELRAttackType GetAttackType() const 
	{ 
		//ID가 캐릭터/적 타입이 아니면 무시
		if (Domain != ELRDomain::CHARACTER && Domain != ELRDomain::ENEMY) return ELRAttackType::NONE;
		return static_cast<ELRAttackType>((FullID / 10000) % 100); 
	}
	
	//[캐릭터] 클래스 반환(5-6번)
	ELRClassType GetClassType() const 
	{
		if (Domain != ELRDomain::CHARACTER) return ELRClassType::NONE;
		return static_cast<ELRClassType>((FullID / 100) % 100);
	}
	
	//[적] 클래스 반환(5-6번)
	ELREnemyType GetEnemyType() const
	{
		if (Domain != ELRDomain::ENEMY) return ELREnemyType::NONE;
		return static_cast<ELREnemyType>((FullID / 100) % 100);
	}
	
	//[장비] 카테고리 반환(3번)
	ELRItemType GetItemType() const
	{
		if (Domain != ELRDomain::EQUIPMENT) return ELRItemType::NONE;
		return static_cast<ELRItemType>((FullID / 100000) % 10);
	}
	
	//[장비] 세트 번호 반환(4-5번)
	ELRSetItemType GetSetItemType() const
	{
		if (Domain != ELRDomain::EQUIPMENT) return ELRSetItemType::NONE;
		return static_cast<ELRSetItemType>((FullID/1000) % 100);
	}
	
};