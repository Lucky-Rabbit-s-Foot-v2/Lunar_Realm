// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LROutGameController.h"

#include "System/LoggingSystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Lobby/LRLobbyWidget.h"
#include "UI/Gacha/LRGachaShopWidget.h"
#include "UI/Chapter/LRChapterPageWidget.h"
#include "UI/Chapter/LRStagePopupWidget.h"


void ALROutGameController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->SwitchPageUIByID(EUIID::LOBBY);
}

void ALROutGameController::SetSelectedCharacterID(FName InID)
{
	SelectedCharacterID = InID;
	OnSelectedCharacterChangedDel.Broadcast(SelectedCharacterID);
}

void ALROutGameController::GachaSim(const FString& BannerIdStr, int32 TotalPulls, int32 Seed)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	ULRGachaSubsystem* Gacha = GI->GetSubsystem<ULRGachaSubsystem>();
	if (!Gacha) return;

	const FName BannerID(*BannerIdStr);

	FLRGachaSimSummary NoPity;
	FLRGachaSimSummary WithPity;

	// 1) 천장 OFF
	Gacha->Debug_SimulateBanner(BannerID, TotalPulls, Seed, NoPity, true, false);

	// 2) 천장 ON
	Gacha->Debug_SimulateBanner(BannerID, TotalPulls, Seed, WithPity, true, true);

	// 로그
	UE_LOG(LogTemp, Warning, TEXT("[GachaSimCompare] Banner=%s Seed=%d Pulls=%d"), *BannerID.ToString(), Seed, TotalPulls);
	UE_LOG(LogTemp, Warning, TEXT("  NoPity  : %s"), *NoPity.ToString());
	UE_LOG(LogTemp, Warning, TEXT("  WithPity: %s"), *WithPity.ToString());
}
