// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartySlotsWidget.h"

#include "Components/Image.h"

#include "Engine/Texture2D.h"
#include "Engine/GameInstance.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRPartySlotWidget.h"

void ULRPartySlotsWidget::InitializeUI()
{
	Super::InitializeUI();

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		TArray<FName> PartyCharactersIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();
		for (int32 SlotIdx = 0; SlotIdx < 5; SlotIdx++)
		{
			if (SubWidgets.IsValidIndex(SlotIdx) && PartyCharactersIDs.IsValidIndex(SlotIdx))
			{
				if (ULRPartySlotWidget* CurrentSlot = Cast<ULRPartySlotWidget>(SubWidgets[SlotIdx]))
				{
					CurrentSlot->SetCharacterID(PartyCharactersIDs[SlotIdx]);
				}
			}
		}
	}
}

void ULRPartySlotsWidget::BindProperties()
{
	Super::BindProperties();
}

void ULRPartySlotsWidget::UnbindProperties()
{
	Slot_Main->OnPartySlotChangedDel.Clear();
	Slot_1->OnPartySlotChangedDel.Clear();
	Slot_2->OnPartySlotChangedDel.Clear();
	Slot_3->OnPartySlotChangedDel.Clear();
	Slot_4->OnPartySlotChangedDel.Clear();

	Super::UnbindProperties();
}

void ULRPartySlotsWidget::BindSubWidgets()
{
	Super::BindSubWidgets();

	Slot_Main->OnPartySlotChangedDel.AddDynamic(this, &ULRPartySlotsWidget::RefreshPartySlots);
	Slot_1->OnPartySlotChangedDel.AddDynamic(this, &ULRPartySlotsWidget::RefreshPartySlots);
	Slot_2->OnPartySlotChangedDel.AddDynamic(this, &ULRPartySlotsWidget::RefreshPartySlots);
	Slot_3->OnPartySlotChangedDel.AddDynamic(this, &ULRPartySlotsWidget::RefreshPartySlots);
	Slot_4->OnPartySlotChangedDel.AddDynamic(this, &ULRPartySlotsWidget::RefreshPartySlots);
}

void ULRPartySlotsWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Slot_Main);
	SubWidgets.Add(Slot_1);
	SubWidgets.Add(Slot_2);
	SubWidgets.Add(Slot_3);
	SubWidgets.Add(Slot_4);
}

void ULRPartySlotsWidget::RefreshPartySlots()
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	TArray<FName> PartyCharactersIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();

	for (int32 SlotIdx = 0; SlotIdx < 5; SlotIdx++)
	{
		if (SubWidgets.IsValidIndex(SlotIdx))
		{
			if (ULRPartySlotWidget* CurrentSlot = Cast<ULRPartySlotWidget>(SubWidgets[SlotIdx]))
			{
				CurrentSlot->SetCharacterID(PartyCharactersIDs.IsValidIndex(SlotIdx) ? PartyCharactersIDs[SlotIdx] : NAME_None);
				CurrentSlot->RefreshUI();
			}
		}
	}
}
