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

#include "UI/Chapter/LRReadySlot.h"

void ULRPartyLineupWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MemberSlots.Empty();
	MemberSlots.Add(Main);
	MemberSlots.Add(Member1);
	MemberSlots.Add(Member2);
	MemberSlots.Add(Member3);
	MemberSlots.Add(Member4);
}

void ULRPartyLineupWidget::RefreshUI()
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

	TArray<FName> PartyCharacterIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();
	for (int32 i = 0; i < MemberSlots.Num(); i++)
	{
		FName CharacterID = PartyCharacterIDs.IsValidIndex(i) ? PartyCharacterIDs[i] : NAME_None;

		UTexture2D* Portrait = nullptr;
		UTexture2D* Grade = nullptr;
		UTexture2D* Equip = nullptr;

		if (CharacterID.IsNone())
		{
			Portrait = EmptySlotTexture;
			Grade = nullptr;
		}
		else
		{
			Portrait = GameDataSubsystem->GetCharacterStaticData(CharacterID).PortraitIcon.LoadSynchronous();
			Grade = GameDataSubsystem->GetCharacterStaticData(CharacterID).GradeImage.LoadSynchronous();
		}

		FGuid Guid = SaveGameSubsystem->GetLeaderEquipmentID(i);
		if (!Guid.IsValid())
		{
			Equip = EmptySlotTexture;
		}
		else
		{
			FEquipmentInstance EquipmentInstance = CollectionSubsystem->GetEquipmentInstance(Guid);
			const FEquipmentStaticData& EquipmentData = GameDataSubsystem->GetEquipmentStaticData(EquipmentInstance.EquipmentID);
			if (EquipmentData.DataID.IsNone())
			{
				Equip = EmptySlotTexture;
			}
			else
			{
				Equip = EquipmentData.EquipmentTexture.LoadSynchronous();
			}
		}

		MemberSlots[i]->SetData(Portrait, Grade, Equip);
	}
}
