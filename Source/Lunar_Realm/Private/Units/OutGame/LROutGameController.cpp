// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LROutGameController.h"

#include "System/LoggingSystem.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "UI/OutGame/LRLobbyWidget.h"
#include "UI/Gacha/LRGachaShopWidget.h"



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

void ALROutGameController::OpenLobbyWidget()
{
	if (LobbyWidgetClass)
	{
		OpenWidget<ULRLobbyWidget>(LobbyWidgetClass);
	}
	else
	{
		LR_FATAL(TEXT("LobbyWidgetClass is not set in LROutGameController"));
	}
}

void ALROutGameController::OpenShopWidget()
{
	if (ShopWidgetClass)
	{
		OpenWidget<UBaseWidget>(ShopWidgetClass);
	}
	else
	{
		LR_FATAL(TEXT("ShopWidgetClass is not set in LROutGameController"));
	}
}

void ALROutGameController::OpenShopWidgetByCurrency()
{
	if (ShopWidgetClass)
	{
		OpenWidget<UBaseWidget>(ShopWidgetClass);
	}
	else
	{
		LR_FATAL(TEXT("ShopWidgetClass is not set in LROutGameController"));
	}
}

void ALROutGameController::OpenGachaShopWidget()
{
	if (GachaShopWidgetClass)
	{
		OpenWidget<ULRGachaShopWidget>(GachaShopWidgetClass);
	}
	else
	{
		LR_FATAL(TEXT("GachaShopWidgetClass is not set in LROutGameController"));
	}
}

void ALROutGameController::OpenCollectionWidget()
{
	// TODO : 구현 필요
}

void ALROutGameController::OpenPartyWidget()
{
	// TODO : 구현 필요
}

void ALROutGameController::OpenChapterWidget()
{
	// TODO : 구현 필요
}

void ALROutGameController::OpenStageWidget()
{
	// TODO : 구현 필요
}

void ALROutGameController::OpenSettingsWidget()
{
	// TODO : 구현 필요
}

void ALROutGameController::OpenFigureInfo(FName CharacterID)
{
	// TODO : 구현 필요
}

void ALROutGameController::CloseFigureInfo()
{
	// TODO : 구현 필요
}

