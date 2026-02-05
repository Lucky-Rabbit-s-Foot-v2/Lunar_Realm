// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

void ALRPlayerController::GachaSim(const FString& BannerIdStr, int32 TotalPulls, int32 Seed)
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

