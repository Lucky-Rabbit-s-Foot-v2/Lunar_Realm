// Fill out your copyright notice in the Description page of Project Settings.


#include "System/LRCheatManager.h"

#include "Engine/GameInstance.h"

#include "Subsystems/StageManagerSubsystem.h"
#include "Subsystems/CurrencySubsystem.h"

#include "Core/Stage/LRStageGameMode.h"

void ULRCheatManager::ToggleStages()
{
	if (UStageManagerSubsystem* StageMgr = GetWorld()->GetGameInstance()->GetSubsystem<UStageManagerSubsystem>())
	{
		bool bIsUnlocked = StageMgr->IsCheatStageUnlocked();
		StageMgr->SetCheatStageUnlocked(!bIsUnlocked);
	}
}

void ULRCheatManager::UnlockStages()
{
	if (UStageManagerSubsystem* StageMgr = GetWorld()->GetGameInstance()->GetSubsystem<UStageManagerSubsystem>())
	{
		StageMgr->SetCheatStageUnlocked(true);
	}
}

void ULRCheatManager::LockStages()
{
	if (UStageManagerSubsystem* StageMgr = GetWorld()->GetGameInstance()->GetSubsystem<UStageManagerSubsystem>())
	{
		StageMgr->SetCheatStageUnlocked(false);
	}
}

void ULRCheatManager::GameClear()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnGameClear();
	}
}

void ULRCheatManager::GameOver()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnGameOver();
	}
}

void ULRCheatManager::ShowMeTheMoney()
{
	AddGold(9999);
	AddCrescentTicket(999);
	AddFullMoonTicket(999);
}

void ULRCheatManager::AddGold(int32 Amount)
{
	if (UCurrencySubsystem* CurrencySS = GetWorld()->GetGameInstance()->GetSubsystem<UCurrencySubsystem>())
	{
		CurrencySS->AddCurrency(ELRCurrencyType::Gold, Amount);
	}
}

void ULRCheatManager::AddCrescentTicket(int32 Amount)
{
	if (UCurrencySubsystem* CurrencySS = GetWorld()->GetGameInstance()->GetSubsystem<UCurrencySubsystem>())
	{
		CurrencySS->AddCurrency(ELRCurrencyType::CrescentTicket, Amount);
	}
}

void ULRCheatManager::AddFullMoonTicket(int32 Amount)
{
	if (UCurrencySubsystem* CurrencySS = GetWorld()->GetGameInstance()->GetSubsystem<UCurrencySubsystem>())
	{
		CurrencySS->AddCurrency(ELRCurrencyType::FullMoonTicket, Amount);
	}
}
