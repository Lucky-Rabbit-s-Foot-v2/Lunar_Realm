// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartySlotsWidget.h"

#include "Components/Image.h"

#include "Engine/Texture2D.h"
#include "Engine/GameInstance.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRPartySlotWidget.h"

#include "UI/Core/LRButtonWidget.h"

void ULRPartySlotsWidget::InitializeUI()
{
	Super::InitializeUI();

	Slot0->SetSlotIndex(0);
	Slot1->SetSlotIndex(1);
	Slot2->SetSlotIndex(2);
	Slot3->SetSlotIndex(3);
	Slot4->SetSlotIndex(4);
}

void ULRPartySlotsWidget::BindProperties()
{
	Super::BindProperties();
	
	Btn_Mount->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::OnPartyMountClicked);
	Btn_Swap->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::OnPartySwapClicked);
	Btn_Release->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::OnPartyReleaseClicked);
}

void ULRPartySlotsWidget::UnbindProperties()
{
	Btn_Mount->OnLRButtonClickedDel.RemoveDynamic(this, &ULRPartySlotsWidget::OnPartyMountClicked);
	Btn_Swap->OnLRButtonClickedDel.RemoveDynamic(this, &ULRPartySlotsWidget::OnPartySwapClicked);
	Btn_Release->OnLRButtonClickedDel.RemoveDynamic(this, &ULRPartySlotsWidget::OnPartyReleaseClicked);
	
	Super::UnbindProperties();
}

void ULRPartySlotsWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Slot0);
	SubWidgets.Add(Slot1);
	SubWidgets.Add(Slot2);
	SubWidgets.Add(Slot3);
	SubWidgets.Add(Slot4);
}

void ULRPartySlotsWidget::OnPartyMountClicked()
{
	ButtonType = ESelectedButtonType::MOUNT;
}

void ULRPartySlotsWidget::OnPartySwapClicked()
{
	ButtonType = ESelectedButtonType::SWAP;
}

void ULRPartySlotsWidget::OnPartyReleaseClicked()
{
	ButtonType = ESelectedButtonType::CLEAR;
}
