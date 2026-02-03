// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "Data/Gacha/LRGachaTransactionTypes.h"
#include "LRGachaSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGachaSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 재화들(골드/티켓) 저장
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FGameplayTag, int32> CurrencyMap;

	//배너별 천장 카운트 저장 (보름달 배너만 사용해도 됨)
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FName, int32> PityCounterMap;

	// 진행 중 트랜잭션(1개만 운영할 거라 Key는 하나만 있어도 되지만 확장용으로 Map)
	UPROPERTY(SaveGame, BlueprintReadWrite)
	TMap<FGuid, FLRGachaPendingTransaction> PendingTransactions;
	
};
