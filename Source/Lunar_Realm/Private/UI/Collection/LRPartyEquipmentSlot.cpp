// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyEquipmentSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULRPartyEquipmentSlot::SetSlotIndex(int32 InIndex)
{
	Super::SetSlotIndex(InIndex);

	Type = ECollectionType::EQUIPMENT;

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

	FGuid EquipmentGuid = SaveGameSubsystem->GetLeaderEquipmentID(SlotIndex);
	if (!EquipmentGuid.IsValid())
	{
		return;
	}

	FEquipmentInstance EquipmentInstances = CollectionSubsystem->GetEquipmentInstance(EquipmentGuid);
	SetID(EquipmentInstances.EquipmentID);
}

void ULRPartyEquipmentSlot::SetID(FName InID)
{
	Super::SetID(InID);

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

	TArray<FEquipmentInstance> EquipmentInstances =	CollectionSubsystem->GetEquipmentInstancesByKey(ID);
	
	if (EquipmentInstances.Num() > 0)
	{
		SaveGameSubsystem->SetLeaderEquipmentSlot(SlotIndex, EquipmentInstances[0].InstanceID);
	}
}

void ULRPartyEquipmentSlot::SetGradeImage()
{
	Super::SetGradeImage();

	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FEquipmentStaticData& StaticData = GameDataSubsystem->GetEquipmentStaticData(ID);
		
		Img_Grade->SetVisibility(ESlateVisibility::Hidden);
		//Img_Grade->SetBrushFromTexture(StaticData.GradeTexture.LoadSynchronous());
	}
}

void ULRPartyEquipmentSlot::SetIconImage()
{
	Super::SetIconImage();

	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		const FEquipmentStaticData& StaticData = GameDataSubsystem->GetEquipmentStaticData(ID);
		Image->SetBrushFromTexture(StaticData.EquipmentTexture.LoadSynchronous());
	}
}