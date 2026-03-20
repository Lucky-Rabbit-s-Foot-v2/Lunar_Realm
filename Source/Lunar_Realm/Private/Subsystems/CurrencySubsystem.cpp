// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Engine/GameInstance.h"

// ───────────────── UGameInstanceSubsystem 구현 ─────────────────

void UCurrencySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 지금은 추가 초기화 로직 없음.
	// 나중에 재화 관련 Event 바인딩/로깅 등을 추가하려면 여기에서 처리.
}

// ───────────────── 내부 헬퍼 ─────────────────

USaveGameSubsystem* UCurrencySubsystem::GetSaveGameSubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<USaveGameSubsystem>();
	}
	return nullptr;
}

// ───────────────── 재화 API ─────────────────

int32 UCurrencySubsystem::GetCurrency(ELRCurrencyType Type) const
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem())
	{
		return SaveGameSubsystem->GetCurrency(Type);
	}
	return 0;
}

void UCurrencySubsystem::AddCurrency(ELRCurrencyType Type, int32 Amount)
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem())
	{
		SaveGameSubsystem->AddCurrency(Type, Amount);
		OnCurrencyChangedDel.Broadcast();
	}
}

bool UCurrencySubsystem::SpendCurrency(ELRCurrencyType Type, int32 Amount)
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem())
	{
		bool bSuccess = SaveGameSubsystem->TrySpendCurrency(Type, Amount);
		OnCurrencyChangedDel.Broadcast();
		return bSuccess;
	}
	return false;
}