// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRPartyLineupWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

void ULRPartyLineupWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MemberImages.Empty();
	MemberImages.Add(Img_Main);
	MemberImages.Add(Img_Member1);
	MemberImages.Add(Img_Member2);
	MemberImages.Add(Img_Member3);
	MemberImages.Add(Img_Member4);

	EquipImages.Empty();
	EquipImages.Add(Img_Equip1);
	EquipImages.Add(Img_Equip2);
	EquipImages.Add(Img_Equip3);
}

void ULRPartyLineupWidget::RefreshUI()
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	
	TArray<FName> PartyCharacterIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();
	for (int32 i = 0; i < MemberImages.Num(); i++)
	{
		FName CharacterID = PartyCharacterIDs.IsValidIndex(i) ? PartyCharacterIDs[i] : NAME_None;

		if(CharacterID.IsNone())
		{
			MemberImages[i]->SetBrushFromTexture(EmptySlotTexture);
		}
		else
		{
			MemberImages[i]->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[i]).PortraitIcon.LoadSynchronous());
		}
	}

	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	for (int32 i = 0; i < EquipImages.Num(); i++)
	{
		FGuid Guid = SaveGameSubsystem->GetLeaderEquipmentID(i);
		if (!Guid.IsValid())
		{
			EquipImages[i]->SetBrushFromTexture(EmptySlotTexture);
			continue;
		}

		FEquipmentInstance EquipmentInstance = CollectionSubsystem->GetEquipmentInstance(Guid);
		const FEquipmentStaticData& EquipmentData = GameDataSubsystem->GetEquipmentStaticData(EquipmentInstance.EquipmentID);
		if (EquipmentData.DataID.IsNone())
		{
			EquipImages[i]->SetBrushFromTexture(EmptySlotTexture);
		}
		else
		{
			EquipImages[i]->SetBrushFromTexture(EquipmentData.EquipmentTexture.LoadSynchronous());
		}
	}	
}
