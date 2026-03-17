// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRPartyLineupWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRPartyLineupWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MemberImages.Empty();
	MemberImages.Add(Img_Main);
	MemberImages.Add(Img_Member1);
	MemberImages.Add(Img_Member2);
	MemberImages.Add(Img_Member3);
	MemberImages.Add(Img_Member4);
}

void ULRPartyLineupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Regroup) Btn_Regroup->OnClicked.AddDynamic(this, &ULRPartyLineupWidget::OnRegroupButtonClicked);
}

void ULRPartyLineupWidget::UnbindProperties()
{
	if (Btn_Regroup) Btn_Regroup->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPartyLineupWidget::RefreshUI()
{
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	
	TArray<FName> PartyCharacterIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();

	for (int32 i = 0; i < MemberImages.Num(); i++)
	{
		if (PartyCharacterIDs.IsValidIndex(i))
		{
			MemberImages[i]->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[i]).PortraitIcon.LoadSynchronous());
		}
		else
		{
			MemberImages[i]->SetBrushFromTexture(EmptySlotTexture);
		}
	}
	// TODO: 장비 이미지 설정
	TArray<FGuid> LeaderEquipmentIDs = SaveGameSubsystem->GetAllLeaderEquipmentIDs();
}

void ULRPartyLineupWidget::OnRegroupButtonClicked()
{
	UUIManagerSubsystem* UIManagerSubsystem = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManagerSubsystem->OpenUIByID(EUIID::PARTY);
}

