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
		if (ID.IsNone())
		{
			Img_Grade->SetVisibility(ESlateVisibility::Hidden);
			Img_Slot->SetBrushFromTexture(EmptySlotTexture);
			return;
		}

		const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
		Img_Grade->SetVisibility(ESlateVisibility::Visible);
		Img_Grade->SetBrushFromTexture(StaticData.GradeImage.LoadSynchronous());
		Img_Slot->SetBrushFromTexture(StaticData.PortraitIcon.LoadSynchronous());
	}
}

void ULRPartySlotWidget::OnSlotButtonClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		ID = PC->GetSelectedID();
	} 

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		int32 PreviousSlotIndex = -1;

		TArray<FName> AllPartyCharactersIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();
		for (int32 i = 0; i < AllPartyCharactersIDs.Num(); i++)
		{
			if (AllPartyCharactersIDs[i] == ID)
			{
				PreviousSlotIndex = i;
				break;
			}
		}

		if (PreviousSlotIndex != -1)
		{
			FName CachedID = SaveGameSubsystem->GetPartyCharacterID(SlotIndex);
			SaveGameSubsystem->SetPartySlot(PreviousSlotIndex, CachedID);
		}
		SaveGameSubsystem->SetPartySlot(SlotIndex, ID);
	}

	if (OnPartySlotChangedDel.IsBound())
	{
		LR_INFO(TEXT("Party Slot %d changed to Character ID %s"), SlotIndex, *ID.ToString());
		OnPartySlotChangedDel.Broadcast();
	}
}

