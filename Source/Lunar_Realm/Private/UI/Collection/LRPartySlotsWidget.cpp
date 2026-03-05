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

void ULRPartySlotsWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Slot_Main);
	SubWidgets.Add(Slot_1);
	SubWidgets.Add(Slot_2);
	SubWidgets.Add(Slot_3);
	SubWidgets.Add(Slot_4);
}
