// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LROutGameController.h"

#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"


ALROutGameController::ALROutGameController()
	: ALRControllerBase()
{
}

void ALROutGameController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::OUTGAME);
	UIManager->ClearUIHistory();

	UIManager->OpenUIByID(EUIID::LOBBY);

	if (LobbyGachaBGMSound)
	{
		UGameplayStatics::PlaySound2D(this, LobbyGachaBGMSound, 1.0f);
	}
}

void ALROutGameController::SetSelectedCharacterID(FName InID)
{
	SelectedInfo.ID = InID;
	SelectedInfo.Type = ECollectionType::CHARACTER;
	OnSelectedChangedDel.Broadcast(SelectedInfo);
}

void ALROutGameController::SetSelectedEquipmentID(FName InID)
{
	SelectedInfo.ID = InID;
	SelectedInfo.Type = ECollectionType::EQUIPMENT;
	OnSelectedChangedDel.Broadcast(SelectedInfo);
}

FName ALROutGameController::GetSelectedCharacterID()
{
	return SelectedInfo.Type == ECollectionType::CHARACTER ? SelectedInfo.ID : NAME_None;
}

FName ALROutGameController::GetSelectedEquipmentID()
{
	return SelectedInfo.Type == ECollectionType::EQUIPMENT ? SelectedInfo.ID : NAME_None;
}

void ALROutGameController::RequestUpdateSelectedInfo(const FSelectedInfo& InInfo)
{
	SelectedInfo = InInfo;
	OnSelectedChangedDel.Broadcast(SelectedInfo);
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
}
