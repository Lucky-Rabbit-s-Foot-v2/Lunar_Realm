// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/LREnumType.h"
#include "CurrencySubsystem.generated.h"

class USaveGameSubsystem;

/**
 * 재화 전용 GameInstanceSubsystem
 *
 * 책임:
 * - SaveGameSubsystem을 통해 재화 조회/증가/소비를 한 곳에서 처리
 * - 게임 내 다른 시스템은 이 Subsystem만 알면 재화 접근 가능
 */
UCLASS()
class LUNAR_REALM_API UCurrencySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// UGameInstanceSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// ───────────────── 재화 API ─────────────────

	/** 현재 재화 값 조회 */
	UFUNCTION(BlueprintCallable, Category = "LR|Currency")
	int32 GetCurrency(ELRCurrencyType Type) const;

	/** 재화 추가 (양수/음수 모두 허용 가능: SaveGame 쪽 정책 따름) */
	UFUNCTION(BlueprintCallable, Category = "LR|Currency")
	void AddCurrency(ELRCurrencyType Type, int32 Amount);

	/** 재화 소모 (잔액 부족 시 false) */
	UFUNCTION(BlueprintCallable, Category = "LR|Currency")
	bool SpendCurrency(ELRCurrencyType Type, int32 Amount);

private:
	// ───────────────── 내부 헬퍼 ─────────────────

	/** 현재 GameInstance에서 SaveGameSubsystem 가져오기 */
	USaveGameSubsystem* GetSaveGameSubsystem() const;
};