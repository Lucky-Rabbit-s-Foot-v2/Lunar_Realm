#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/Gacha/LRGachaTypes.h"
#include "LRGachaTransactionTypes.generated.h"

// 트랜잭션 상태
UENUM(BlueprintType)
enum class ELRGachaTxnState : uint8
{
	None,
	PendingReveal,	//	결과 확정/저장됨. 리빌 대기
	Committed,		//	지급 완료
	Canceled		//	취소/원복 완료
};

// SaveGame에 저장될 Pending 트랜잭션
USTRUCT(BlueprintType)
struct FLRGachaPendingTransaction
{
	GENERATED_BODY()

	// 트랜잭션 식별자(중복 방지/복구용)
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FGuid TxnId;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FName BannerID;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 DrawCount = 0;

	// 비용 정보(취소 시 환불용)
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FGameplayTag CostCurrencyTag;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 CostAmount = 0;

	// 천장 원복용
	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 PrevPity = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 NewPity = 0;

	// 결과(리빌에 보여줄 것)
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FLRGachaResult> Results;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRGachaTxnState State = ELRGachaTxnState::None;
};