// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"

#include "Data/Gacha/LRGachaTransactionTypes.h"

#include "LRGachaSaveGame.generated.h"

/**
 * 가챠 관련 저장 데이터
 * 재화(골드/티켓)
 * 배너별 천장 카운트
 * 진행 중 트랜잭션(튕김 복구용)
 */

UCLASS()
class LUNAR_REALM_API ULRGachaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 재화들(골드/티켓) 저장
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FGameplayTag, int32> CurrencyMap;

	//배너별 천장 카운트 저장
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FName, int32> PityCounterMap;

	// 진행 중 트랜잭션 (복구/확장용)
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FGuid, FLRGachaPendingTransaction> PendingTransactions;
	
};
