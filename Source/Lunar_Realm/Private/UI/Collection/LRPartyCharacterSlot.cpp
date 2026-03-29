// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyCharacterSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULRPartyCharacterSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SaveGameSubsystem->OnSaveGameSavedDel.AddUniqueDynamic(this, &ULRPartyCharacterSlot::RefreshUICaller);
	}
}

void ULRPartyCharacterSlot::RefreshUI()
{
	Super::RefreshUI();

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	FName CharacterID = SaveGameSubsystem->GetPartyCharacterID(SlotIndex);
	ID = CharacterID;

	SetGradeImage();
	SetIconImage();
}

void ULRPartyCharacterSlot::SetSlotIndex(int32 InIndex)
{
	Super::SetSlotIndex(InIndex);

	Type = ECollectionType::CHARACTER;

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SetID(SaveGameSubsystem->GetPartyCharacterID(SlotIndex));
	}
}

void ULRPartyCharacterSlot::RefreshUICaller()
{
	RefreshUI();
}

void ULRPartyCharacterSlot::SetIDAuto()
{
	Super::SetIDAuto();

	SetSlotIndex(SlotIndex);
}

void ULRPartyCharacterSlot::SetID(FName InID)
{
	Super::SetID(InID);

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	SaveGameSubsystem->SetPartySlot(SlotIndex, InID);

	RefreshUI();
}

void ULRPartyCharacterSlot::SetGradeImage()
{
	Super::SetGradeImage();

	if (ID.IsNone())
	{
		Img_Grade->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
		Img_Grade->SetBrushFromTexture(StaticData.GradeImage.LoadSynchronous());
		Img_Grade->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULRPartyCharacterSlot::SetIconImage()
{
	Super::SetIconImage();

	if (ID.IsNone())
	{
		Image->SetBrushFromTexture(EmptySlotTexture);
		return;
	}
	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
		Image->SetBrushFromTexture(StaticData.PortraitIcon.LoadSynchronous());
	}
}
