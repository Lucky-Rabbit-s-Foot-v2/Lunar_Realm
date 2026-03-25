// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyCharacterSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRPartyCharacterSlot::OnButtonClicked()
{
	Super::OnButtonClicked();
	OnCharacterSlotChangedDel.Broadcast(SlotIndex);
}


void ULRPartyCharacterSlot::RefreshUI()
{
	Super::RefreshUI();

	if (ID.IsNone())
	{
		Img_Grade->SetVisibility(ESlateVisibility::Hidden);
		Image->SetBrushFromTexture(EmptySlotTexture);
		return;
	}

	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
		Img_Grade->SetVisibility(ESlateVisibility::Visible);
		Img_Grade->SetBrushFromTexture(StaticData.GradeImage.LoadSynchronous());
		Image->SetBrushFromTexture(StaticData.PortraitIcon.LoadSynchronous());
	}
}

void ULRPartyCharacterSlot::SetSlotIndex(int32 InIndex)
{
	SlotIndex = InIndex;

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SetCharacterID(SaveGameSubsystem->GetPartyCharacterID(SlotIndex));
	}
}

void ULRPartyCharacterSlot::SetCharacterID(FName InID)
{
	ID = InID;

	RefreshUI();
}