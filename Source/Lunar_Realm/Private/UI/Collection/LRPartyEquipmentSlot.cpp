// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyEquipmentSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

void ULRPartyEquipmentSlot::OnButtonClicked()
{
	Super::OnButtonClicked();
	OnEquipmentSlotChangedDel.Broadcast(SlotIndex);
}

void ULRPartyEquipmentSlot::RefreshUI()
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
		const FEquipmentStaticData& StaticData = GameDataSubsystem->GetEquipmentStaticData(ID);
		Img_Grade->SetVisibility(ESlateVisibility::Visible);
		//Img_Grade->SetBrushFromTexture(StaticData.GradeImage.LoadSynchronous());
		Image->SetBrushFromTexture(StaticData.EquipmentTexture.LoadSynchronous());
	}
}

void ULRPartyEquipmentSlot::SetSlotIndex(int32 InIndex)
{
	SlotIndex = InIndex;
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

	FGuid EquipmentGuid = SaveGameSubsystem->GetLeaderEquipmentID(SlotIndex);
	if (!EquipmentGuid.IsValid())
	{
		return;
	}

	FEquipmentInstance EquipmentInstances = CollectionSubsystem->GetEquipmentInstance(EquipmentGuid);
	SetEquipID(EquipmentInstances.EquipmentID);
}

void ULRPartyEquipmentSlot::SetEquipID(FName InID)
{
	ID = InID;
	RefreshUI();
}
