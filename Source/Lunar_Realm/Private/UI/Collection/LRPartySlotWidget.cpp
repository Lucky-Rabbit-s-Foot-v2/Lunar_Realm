// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartySlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/Texture2D.h"

#include "Units/OutGame/LROutGameController.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

void ULRPartySlotWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Slot->OnClicked.AddDynamic(this, &ULRPartySlotWidget::OnSlotButtonClicked);
}

void ULRPartySlotWidget::UnbindProperties()
{
	Btn_Slot->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPartySlotWidget::RefreshUI()
{
	Super::RefreshUI();

	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
		Img_Slot->SetBrushFromTexture(StaticData.PortraitIcon.LoadSynchronous());
	}
}

void ULRPartySlotWidget::OnSlotButtonClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		ID = PC->GetSelectedCharacterID();
	}

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SaveGameSubsystem->SetPartySlot(SlotIndex, ID);
	}

	RefreshUI();
}

