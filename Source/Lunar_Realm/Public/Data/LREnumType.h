// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


// =============================================================================
/** 
 * ELRDomain, ELRGrade, ELRAttackType, ELRClassType, ELRItemType, 
 * - ID 파싱 정보 관리를 위한 Enum
 */
//=============================================================================
// (260205) KHS 제작. 제반 사항 구현.
// (260224) KHS 스킬관련 Enum (투사체타입/스킬파라미터타입/버프타입) 추가.
// (260226) KHS 스킬 기획 변경에 따라 Enum추가
// =============================================================================

//ID 타입 식별용
UENUM(BlueprintType)
enum class ELRDomain : uint8
{
	NONE = 0,
	CHARACTER = 1,
	EQUIPMENT = 2,
	ENEMY = 3,
	SKILL = 4,
	
	MAX UMETA(Hidden)
};


//공통 공격스타일(캐릭터/적 공유)
UENUM(BlueprintType)
enum class ELRAttackType : uint8
{
	NONE = 0,
	MELEE = 1,
	RANGED = 2,
	
	MAX UMETA(Hidden)
};


//공통 사용 등급(캐릭터/적/장비 공유)
UENUM(BlueprintType)
enum class ELRGrade : uint8
{
	NONE = 0,
	N = 1,
	R = 2,
	SR = 3,
	SSR = 4,
	UR = 5,

	MAX = 6 UMETA(Hidden)
};

//========================================
/*
 * 캐릭터 관련 Enum
 */
//========================================

//캐릭터 클래스 타입(직업)
UENUM(BlueprintType)
enum class ELRClassType : uint8
{
	NONE = 0,
	MAGICIAN = 1,
	WARRIOR = 2,
	ARCHER = 3,
	
	MAX UMETA(Hidden)
};



//========================================
/*
 * 에너미 관련 Enum
 */
//========================================


//적 클래스 타입(직업)
UENUM(BlueprintType)
enum class ELREnemyType : uint8
{
	NONE = 0,
	GOBLIN = 1, 
	GOLEM = 2,
	SLIME = 3,
	BEAST = 4,
	BAT = 5, 
	GHOST = 6,
	
	MAX UMETA(Hidden)
};

//========================================
/*
 * 장비 관련 Enum
 */
//========================================

//장비 카테고리
UENUM(BlueprintType)
enum class ELRItemType : uint8
{
	NONE = 0,
	MELEE = 1,
	RANGED = 2,
	SIDEARM = 3,
	HELMET = 4,
	ARMOR = 5,
	
	CONSUMABLE = 9,
	
	MAX UMETA(Hidden)
};

//장비 세트 타입
UENUM(BlueprintType)
enum class ELRSetItemType : uint8
{
	NONE = 0,
	BASIC = 1,
	FIRE = 2,
	ICE = 3,
	ELECTRIC = 4,
	
	MAX UMETA(Hidden)
};


// =============================================================================
/** 
 * ELRStatusType, EEquipmentType
 * - 커브 테이블 조회 및 도감 시스템 내부 조회 목적 ENUM
 * - 스테이터스 타입, 장비 슬롯 타입
 */
//=============================================================================
// (260128) KHS 제작. 제반 사항 구현.
// =============================================================================
//캐릭터 스탯 타입
UENUM(BlueprintType)
enum class ELRStatusType : uint8
{
	HP,
	ATK,
	DEF,
	EXP,
	MAX UMETA(Hidden)
};

//장비 슬롯 타입
UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	WEAPON = 0,
	HELMET = 1,
	ARMOR = 2, 
	MAX UMETA(Hidden)
};

// =============================================================================
// (260210) PYI 제작
// (260320) PYI 재화타입 Aether 추가
// =============================================================================
// 재화
// =============================================================================
UENUM(BlueprintType)
enum class ELRCurrencyType : uint8
{
	Gold,
	CrescentTicket,
	FullMoonTicket,
	Aether
};

// =============================================================================
// Gacha Enums
// =============================================================================
UENUM(BlueprintType)
enum class ELRGachaItemType : uint8
{
	Hero		UMETA(DisplayName = "Hero"),
	Equipment	UMETA(DisplayName = "Equipment"),
};

UENUM(BlueprintType)
enum class ELRGachaRarity : uint8
{
	N   UMETA(DisplayName = "N"),
	R   UMETA(DisplayName = "R"),
	SR  UMETA(DisplayName = "SR"),
	SSR UMETA(DisplayName = "SSR"),
	UR  UMETA(DisplayName = "UR"),
};

UENUM(BlueprintType)
enum class ELRGachaTicketType : uint8
{
	Crescent UMETA(DisplayName = "Crescent"),
	FullMoon UMETA(DisplayName = "FullMoon"),
};

// =============================================================================
// 가챠 트랜잭션 상태
// =============================================================================
UENUM(BlueprintType)
enum class ELRGachaTxnState : uint8
{
	None,
	PendingReveal,
	Committed,
	Canceled
};

// ============================================================
// 스킬 관련 Enum
// (260224) KHS 추가
// (260026) KHS 변경
// ============================================================

// 투사체 비행 타입(기존 ESkillType -> 이름 변경)
UENUM(BlueprintType)
enum class EFlightType : uint8
{
	LINEAR      UMETA(DisplayName = "직선형"),
	HOMING      UMETA(DisplayName = "유도형"),
	ARC         UMETA(DisplayName = "궤도형"),
	PIERCE      UMETA(DisplayName = "관통형"),
	EXPLODE     UMETA(DisplayName = "폭발형")
};

// 투사체 타격 타입
UENUM(BlueprintType)
enum class EHitType : uint8
{
	SINGLE  UMETA(DisplayName = "단일 타격"),
	PIERCE  UMETA(DisplayName = "관통 타격"),
	AREA    UMETA(DisplayName = "범위 타격")
};

// 투사체 소멸 조건
UENUM(BlueprintType)
enum class EExpireCondition : uint8
{
	OnHit           UMETA(DisplayName = "충돌 시"),
	OnPierceCount   UMETA(DisplayName = "관통 횟수 소진 시"),
	OnExplosion     UMETA(DisplayName = "폭발 후"),
	OnLifetime      UMETA(DisplayName = "수명 만료 시"),
	OnBoundaryExit  UMETA(DisplayName = "맵 경계 이탈 시")
};

// 스폰 패턴
UENUM(BlueprintType)
enum class ESpawnPattern : uint8
{
	SINGLE      UMETA(DisplayName = "단일"),
	SPREAD   UMETA(DisplayName = "부채꼴"),
	CIRCLE      UMETA(DisplayName = "원형")
};


UENUM(BlueprintType)
enum class EStatusType : uint8
{
	BUFF    UMETA(DisplayName = "버프"),
	DEBUFF  UMETA(DisplayName = "디버프")
};

UENUM(BlueprintType)
enum class ECollectionType : uint8
{
	NONE = 255,
	CHARACTER = 0,
	EQUIPMENT = 1,
};

UENUM(BlueprintType)
enum class EPartyTaskType : uint8
{
	NONE = 255,
	MOUNT = 0,
	SWAP = 1,
	RELEASE = 2,
};
