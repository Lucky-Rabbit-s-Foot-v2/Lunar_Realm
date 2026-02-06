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
// =============================================================================

//ID 타입 식별용
UENUM(BlueprintType)
enum class ELRDomain : uint8
{
	NONE = 0,
	CHARACTER = 1,
	EQUIPMENT = 2,
	ENEMY = 3,
	
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

//적 등급
UENUM(BlueprintType)
enum class ELRGrade : uint8
{
	NONE = 0,
	BASIC = 1,
	ELITE = 2,
	UNIQUE = 3,
	EPIC = 4,
	LEGEND = 5,
	
	MAX UMETA(Hidden)
};

//적 클래스 타입(직업)
UENUM(BlueprintType)
enum class ELREnemyType : uint8
{
	NONE = 0,
	GOBLIN = 1, 
	GOLEM = 2,
	SLIME = 3,
	
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