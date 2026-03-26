// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LROutGameController.h"

#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "UI/Collection/LREnhancePageWidget.h"

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
	if (IsSlotSelected(InInfo))
	{
		if (IsSlotSelected(SelectedInfo))
		{
			LR_WARN(TEXT("Both source and target are slots. Performing swap action."));
			HandleSwapAction(SelectedInfo.SlotIndex, InInfo.SlotIndex);
			ResetSelectedInfo();
			OnSlotSelectedDel.Broadcast(false);
		}
		else
		{
			LR_WARN(TEXT("Slot selected. Performing mount action."));
			ResetSelectedInfo();
			SelectedInfo = InInfo;
			OnSlotSelectedDel.Broadcast(true);
		}
	}

	if (IsCellSelected(InInfo))
	{
		OnSlotSelectedDel.Broadcast(false);

		if (IsSlotSelected(SelectedInfo)) 
		{
			LR_WARN(TEXT("Cell selected while a slot is already selected. Performing mount action."));
			HandleMountAction(InInfo, SelectedInfo);
			ResetSelectedInfo();
		}
		else
		{
			LR_WARN(TEXT("Cell selected. Updating selected info."));
			ResetSelectedInfo();
			SelectedInfo = InInfo;
		}
		OnSelectedChangedDel.Broadcast(SelectedInfo);
	}

	return;
}

bool ALROutGameController::IsSlotSelected(const FSelectedInfo& InInfo)
{
	return InInfo.SlotIndex >= 0;
}

bool ALROutGameController::IsTaskSelected(const FSelectedInfo& InInfo)
{
	return InInfo.Task != EPartyTaskType::NONE;
}

bool ALROutGameController::IsCellSelected(const FSelectedInfo& InInfo)
{
	return InInfo.ID != NAME_None && InInfo.SlotIndex == -1;
}

void ALROutGameController::HandleMountAction(const FSelectedInfo& Target, const FSelectedInfo& Source)
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	
	for (int32 i = 0; i < 5; ++i)
	{
		FName CharacterID = SaveGameSubsystem->GetPartyCharacterID(i);
		if(CharacterID == Source.ID)
		{
			HandleSwapAction(i, Source.SlotIndex);
			return;
		}
	}

	SaveGameSubsystem->SetPartySlot(Target.SlotIndex, Source.ID);
}

void ALROutGameController::HandleSwapAction(int32 Slot1, int32 Slot2)
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	FName ID1 = SaveGameSubsystem->GetPartyCharacterID(Slot1);
	FName ID2 = SaveGameSubsystem->GetPartyCharacterID(Slot2);
	SaveGameSubsystem->SetPartySlot(Slot1, ID2);
	SaveGameSubsystem->SetPartySlot(Slot2, ID1);
}

void ALROutGameController::OpenEnhancePage()
{
	UUIManagerSubsystem* UIManagerSubsystem = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (UIManagerSubsystem)
	{
		ULRBaseWidget* Widget = UIManagerSubsystem->OpenUIByID(EUIID::ENHANCE);
		if (!Widget)
		{
			return;
		}
		if (ULREnhancePageWidget* EnhanceWidget = Cast<ULREnhancePageWidget>(Widget))
		{
			EnhanceWidget->SetIDByType(SelectedInfo);
		}
	}
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
