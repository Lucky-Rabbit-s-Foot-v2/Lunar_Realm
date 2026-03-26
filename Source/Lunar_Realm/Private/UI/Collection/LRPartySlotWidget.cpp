// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartySlotWidget.h"

#include "UI/Collection/LRPartyCharacterSlot.h"
#include "UI/Collection/LRPartyEquipmentSlot.h"

void ULRPartySlotWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterSlot);
	SubWidgets.Add(EquipmentSlot);
}

void ULRPartySlotWidget::SetSlotIndex(int32 InIndex)
{
	SlotIndex = InIndex;
	CharacterSlot->SetSlotIndex(SlotIndex);
	EquipmentSlot->SetSlotIndex(SlotIndex);
}
