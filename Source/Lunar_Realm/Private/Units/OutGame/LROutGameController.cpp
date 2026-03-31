// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/OutGame/LROutGameController.h"

#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

#include "Core/LRGameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "UI/Collection/LREnhancePageWidget.h"
#include "UI/Collection/LRCharacterCollection.h"

#include "UI/Collection/LRCharacterEntryWidget.h"
#include "UI/Collection/LREquipEntryWidget.h"
#include "UI/Collection/LRPartyCharacterSlot.h"
#include "UI/Collection/LRPartyEquipmentSlot.h"

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
	
	if (ULRGameInstance* GI = GetGameInstance<ULRGameInstance>())
	{
		EUIID NextUIID = GI->GetNextUIID();
		UIManager->OpenUIByID(NextUIID);
		GI->SetNextUIID(EUIID::LOBBY);
	}

	if (LobbyGachaBGMSound)
	{
		UGameplayStatics::PlaySound2D(this, LobbyGachaBGMSound, 1.0f);
	}
}

void ALROutGameController::OnSelectedEntryWidget(ULREntryWidget* InWidget)
{
	if (SelectedWidget.IsValid())
	{
		ResetWidgetEffect(SelectedWidget.Get());

		if (ULRSlotWidget* CurrentSlotWidget = Cast<ULRSlotWidget>(SelectedWidget.Get()))
		{
			// 기존_슬롯 , 새_엔트리
			ResetWidgetEffect(InWidget);

			HandleMountAction(
				CurrentSlotWidget->GetSlotIndex(),
				InWidget->GetTileData()->Type,
				InWidget->GetTileData()->ID
			);

			SetSelectedEntry(nullptr);
		}
		else if (ULREntryWidget* CurrentEntryWidget = Cast<ULREntryWidget>(SelectedWidget.Get()))
		{
			// 기존_엔트리 , 새_엔트리
			if (SelectedTileData.IsValid())
			{
				SelectedTileData->bIsSelected = false;
				SelectedTileData = nullptr;
			}

			SetSelectedEntry(InWidget);
		}
	}
	else
	{
		// 기존_없음 , 새_엔트리
		SetSelectedEntry(InWidget);
	}
	OnSelectedChangedDel.Broadcast(SelectedID, SelectedType);
}

void ALROutGameController::SetSelectedEntry(ULREntryWidget* InWidget)
{
	if (InWidget)
	{
		SelectedWidget = InWidget;
		SelectedTileData = InWidget->GetTileData();
		SelectedID = InWidget->GetTileData()->ID;
		SelectedType = InWidget->GetTileData()->Type;
	}
	else
	{
		SelectedWidget = nullptr;
		SelectedTileData = nullptr;
		SelectedID = NAME_None;
		SelectedType = ECollectionType::NONE;
	}
}

void ALROutGameController::OnSelectedSlotWidget(ULRSlotWidget* InWidget)
{
	if (SelectedWidget.IsValid())
	{
		ResetWidgetEffect(SelectedWidget.Get());
		ResetWidgetEffect(InWidget);

		if (ULRSlotWidget* CurrentSlotWidget = Cast<ULRSlotWidget>(SelectedWidget.Get()))
		{
			// 기존_슬롯 , 새_슬롯
			HandleSwapAction(
				CurrentSlotWidget->GetSlotIndex(), 
				InWidget->GetSlotIndex(), 
				InWidget->GetType()
			);
		}
		else if (ULREntryWidget* CurrentEntryWidget = Cast<ULREntryWidget>(SelectedWidget.Get()))
		{
			// 기존_엔트리 , 새_슬롯
			if (SelectedTileData.IsValid())
			{
				SelectedTileData->bIsSelected = false;
				SelectedTileData = nullptr;
			}

			HandleMountAction(
				InWidget->GetSlotIndex(),
				SelectedType,
				SelectedID
			);
		}
		SetSelectedSlot(nullptr);
	}
	else
	{
		// 기존_없음 , 새_슬롯
		SetSelectedSlot(InWidget);
	}
	OnSelectedChangedDel.Broadcast(SelectedID, SelectedType);
}

void ALROutGameController::SetSelectedSlot(ULRSlotWidget* InWidget)
{
	if (InWidget)
	{
		SelectedWidget = InWidget;
		SelectedID = InWidget->GetID();
		SelectedType = InWidget->GetType();
	}
	else
	{
		SelectedWidget = nullptr;
		SelectedID = NAME_None;
		SelectedType = ECollectionType::NONE;
	}
}

void ALROutGameController::OnSelectedSlotToggled(bool bIsSelected)
{
	OnButtonVisibleDel.Broadcast(bIsSelected);
}

void ALROutGameController::HandleMountAction(int32 InTargetIndex, ECollectionType InType, FName InID)
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	
	if (InType == ECollectionType::CHARACTER)
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
	else if (InType == ECollectionType::EQUIPMENT)
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
			ResetSelectedWidget();
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
	
		ResetSelectedWidget();
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
	SelectedID = NAME_None;
	SelectedType = ECollectionType::NONE;
}

void ALROutGameController::ResetWidgetEffect(ULRBaseWidget* Widget)
{
	if (ULREntryWidget* EntryWidget = Cast<ULREntryWidget>(Widget))
	{
		EntryWidget->SetSelected(false);
	}

	if (ULRSlotWidget* SlotWidget = Cast<ULRSlotWidget>(Widget))
	{
		SlotWidget->SetSelected(false);
	}
}

void ALROutGameController::ResetSelectedWidget()
{
	if (SelectedWidget.IsValid())
	{
		ResetWidgetEffect(SelectedWidget.Get());
		SelectedWidget = nullptr;
	}
	OnButtonVisibleDel.Broadcast(false);
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
