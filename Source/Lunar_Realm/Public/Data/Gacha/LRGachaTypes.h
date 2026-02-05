#pragma once

#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "LRGachaTypes.generated.h"

/*
* 이 파일은 "가챠 시스템에서 공통으로 쓰는 타입을 정의한다.
* 배너/풀/확률/중복보상 DataTable Row 구조체
* 가챠 결과(연출/UI에 넘길 데이터) 구조체
*/

// 가챠 대상 타입(영웅/장비)
UENUM(BlueprintType)
enum class ELRGachaItemType : uint8
{
	Hero		UMETA(DisplayName="Hero"),
	Equipment	UMETA(DisplayName="Equipment"),
};

// 등급(1~5성)
UENUM(BlueprintType)
enum class ELRGachaRarity : uint8
{
	Common		UMETA(DisplayName = "1성(일반)"),
	Uncommon	UMETA(DisplayName = "2성(고급)"),
	Rare		UMETA(DisplayName = "3성(희귀)"),
	Epic		UMETA(DisplayName = "4성(에픽)"),
	Legendary	UMETA(DisplayName = "5성(전설)"),
};

// 티켓 종류
UENUM(BlueprintType)
enum class ELRGachaTicketType : uint8
{
	Crescent UMETA(DisplayName = "Crescent"),
	FullMoon UMETA(DisplayName = "FullMoon"),
};


//배너(뽑기) 설정 DataTable
USTRUCT(BlueprintType)
struct FLRGachaBannerRow : public FTableRowBase
{
	GENERATED_BODY()

	// 배너 식별자(예 : "Hero_Cresent", "Hero_FullMoon")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BannerID;

	// 배너가 뽑는 대상 타입(영웅/장비)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	// 이 배너가 소모하는 재화 태그(초승달/보름달/골드 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CostCurrencyTag;

	// 1회 / 10회 비용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostSingle = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostTen = 10;

	// 천장 사용 여부(보름달은 true, 초승달은 false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsePity = false;

	// 예: 100회에 전설 확정
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PityThreshold = 100;

	// 천장 확정 등급(보통 Legendary)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity PityGuaranteedRarity = ELRGachaRarity::Legendary;
};

// 배너 풀(뽑힐 수 있는 항복) DT
USTRUCT(BlueprintType)
struct FLRGachaPoolRow : public FTableRowBase
{
	GENERATED_BODY()

	// 어떤 배너에 속하는지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BannerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	// 캐릭터ID 또는 장비ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity Rarity = ELRGachaRarity::Common;
};

// 중복 보상(등급별 골드 전환량) DT
USTRUCT(BlueprintType)
struct FLRGachaDuplicateRewardRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity Rarity = ELRGachaRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GoldAmount = 10;
};

// 1회 결과(연출/UI에 넘길 데이터)
USTRUCT(BlueprintType)
struct FLRGachaResult
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 ItemID = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRGachaRarity Rarity = ELRGachaRarity::Common;

	// 새로 획득(도감에 없던 것)인지
	UPROPERTY(SaveGame, BlueprintReadOnly)
	bool bIsNew = false;

	// 중복이라 골드로 변환되었는지
	UPROPERTY(SaveGame, BlueprintReadOnly)
	bool bConvertedToGold = false;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 ConvertedGoldAmount = 0;

};

// 등급별 확률(배너/타입별) DT
USTRUCT(BlueprintType)
struct FLRGachaRarityRateRow : public FTableRowBase
{
	GENERATED_BODY()

	// 어떤 배너에 적용할지 (예: "Hero_Cresent")
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BannerID;

	// Hero / Equipment 구분
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	// 등급
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity Rarity = ELRGachaRarity::Common;

	// 확률 가중치(60, 20, 15, 4, 1 처럼 합 100 권장 / 아니어도 됨)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rate = 0.0f;

};