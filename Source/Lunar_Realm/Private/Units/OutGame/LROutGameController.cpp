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
#include "UI/Collection/LRCharacterCollection.h"

#include "UI/Collection/LRCharacterEntryWidget.h"
#include "UI/Collection/LREquipEntryWidget.h"

#include "UI/Core/LREntryWidget.h"
#include "UI/Core/LRSlotWidget.h"

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

void ALROutGameController::OnSelectedEntryWidget(ULREntryWidget* InWidget)
{
	ResetSelectedWidgetData();

	// 만약 이미지가 뜨지 않길 원한다면 여기서 SelectedID와 SelectedType을 초기화
	SelectedID = InWidget->GetTileData()->ID;
	SelectedType = InWidget->GetTileData()->Type;

	if (ULRSlotWidget* CurrentSlotWidget = Cast<ULRSlotWidget>(SelectedWidget.Get()))
	{
		HandleMountAction(
			CurrentSlotWidget->GetSlotIndex(), 
			InWidget->GetTileData()->Type,
			InWidget->GetTileData()->ID
		);
		SelectedWidget = nullptr;
	}
	else
	{
		SelectedWidget = InWidget;
	}	

	OnSelectedChangedDel.Broadcast(SelectedID, SelectedType);
}

void ALROutGameController::OnSelectedSlotWidget(ULRSlotWidget* InWidget)
{
	ResetSelectedWidgetData();

	// 만약 이미지가 뜨지 않길 원한다면 여기서 SelectedID와 SelectedType을 초기화
	SelectedID = InWidget->GetID();
	SelectedType = InWidget->GetType();

	if(ULRSlotWidget* CurrentSlotWidget = Cast<ULRSlotWidget>(SelectedWidget.Get()))
	{
		HandleSwapAction(CurrentSlotWidget->GetSlotIndex(), InWidget->GetSlotIndex(), InWidget->GetType());
		SelectedWidget = nullptr;
	}
	else if (ULREntryWidget* CurrentEntryWidget = Cast<ULREntryWidget>(SelectedWidget.Get()))
	{
		HandleMountAction(
			InWidget->GetSlotIndex(), 
			CurrentEntryWidget->GetTileData()->Type,
			CurrentEntryWidget->GetTileData()->ID
		);
		SelectedWidget = nullptr;
	}
	else
	{
		SelectedWidget = InWidget;
	}
	
	OnSelectedChangedDel.Broadcast(SelectedID, SelectedType);
}

void ALROutGameController::OnSelectedSlotToggled(bool bIsSelected)
{
	OnButtonVisibleDel.Broadcast(bIsSelected);
}

void ALROutGameController::HandleMountAction(int32 InTargetIndex, ECollectionType InTargetType, FName InID)
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	
	if (InTargetType == ECollectionType::CHARACTER)
	{
		for (int32 i = 0; i < 5; ++i)
		{
			FName CharacterID = SaveGameSubsystem->GetPartyCharacterID(i);
			if (CharacterID == InID)
			{
				HandleSwapAction(i, InTargetIndex, ECollectionType::CHARACTER);
				return;
			}
		}
		SaveGameSubsystem->SetPartySlot(InTargetIndex, InID);
	}
	else if (InTargetType == ECollectionType::EQUIPMENT)
	{
		UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

		for (int32 i = 0; i < 5; ++i)
		{
			FGuid Guid = SaveGameSubsystem->GetLeaderEquipmentID(i);
			if (InID == CollectionSubsystem->GetEquipmentInstance(Guid).EquipmentID)
			{
				HandleSwapAction(i, InTargetIndex, ECollectionType::EQUIPMENT);
				return;
			}
		}

		TArray<FEquipmentInstance> Instances = CollectionSubsystem->GetEquipmentInstancesByKey(InID);
		if (Instances.Num() > 0)
		{
			SaveGameSubsystem->SetLeaderEquipmentSlot(InTargetIndex, Instances[0].InstanceID);
		}
	}
}

void ALROutGameController::HandleSwapAction(int32 Slot1, int32 Slot2, ECollectionType InType)
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();

	if(InType == ECollectionType::CHARACTER)
	{
		FName ID1 = SaveGameSubsystem->GetPartyCharacterID(Slot1);
		FName ID2 = SaveGameSubsystem->GetPartyCharacterID(Slot2);
		SaveGameSubsystem->SetPartySlot(Slot1, ID2);
		SaveGameSubsystem->SetPartySlot(Slot2, ID1);
	}
	else if(InType == ECollectionType::EQUIPMENT)
	{
		FGuid Guid1 = SaveGameSubsystem->GetLeaderEquipmentID(Slot1);
		FGuid Guid2 = SaveGameSubsystem->GetLeaderEquipmentID(Slot2);
		SaveGameSubsystem->SetLeaderEquipmentSlot(Slot1, Guid2);
		SaveGameSubsystem->SetLeaderEquipmentSlot(Slot2, Guid1);
	}
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
			SelectedWidget = nullptr;
			EnhanceWidget->SetIDAndType(SelectedID, SelectedType);
		}
	}
}

void ALROutGameController::ReleasePartySlot()
{
	if (ULRSlotWidget* SelectedSlot = Cast<ULRSlotWidget>(SelectedWidget.Get()))
	{
		USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();

		SelectedSlot->SetID(NAME_None);
		SelectedSlot->SetSelected(false);
		SelectedSlot->RefreshUI();
	}
}

void ALROutGameController::SetIDAndType(FName InID, ECollectionType InType)
{
	SelectedID = InID;
	SelectedType = InType;
	OnSelectedChangedDel.Broadcast(SelectedID, SelectedType);
}

void ALROutGameController::ResetSelectedData()
{
	ResetSelectedWidgetData();

	SelectedID = NAME_None;
	SelectedType = ECollectionType::NONE;
}

void ALROutGameController::ResetSelectedWidgetData()
{
	if (ULREntryWidget* EntryWidget = Cast<ULREntryWidget>(SelectedWidget.Get()))
	{
		EntryWidget->SetSelected(false);
	}

	if (ULRSlotWidget* SlotWidget = Cast<ULRSlotWidget>(SelectedWidget.Get()))
	{
		SlotWidget->SetSelected(false);
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
