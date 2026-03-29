// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyCharacterSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"


void ULRPartyCharacterSlot::SetSlotIndex(int32 InIndex)
{
	Super::SetSlotIndex(InIndex);

	Type = ECollectionType::CHARACTER;

	if (USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		SetID(SaveGameSubsystem->GetPartyCharacterID(SlotIndex));
	}
}

void ULRPartyCharacterSlot::SetID(FName InID)
{
	Super::SetID(InID);

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	SaveGameSubsystem->SetPartySlot(SlotIndex, InID);
}

void ULRPartyCharacterSlot::SetGradeImage()
{
	Super::SetGradeImage();

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

	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
		Image->SetBrushFromTexture(StaticData.PortraitIcon.LoadSynchronous());
	}
}
