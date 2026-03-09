// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRPartyLineupWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

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

	Img_Main->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[0]).PortraitIcon.LoadSynchronous());
	Img_Member1->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[1]).PortraitIcon.LoadSynchronous());
	Img_Member2->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[2]).PortraitIcon.LoadSynchronous());
	Img_Member3->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[3]).PortraitIcon.LoadSynchronous());
	Img_Member4->SetBrushFromTexture(GameDataSubsystem->GetCharacterStaticData(PartyCharacterIDs[4]).PortraitIcon.LoadSynchronous());
	
	// TODO: 장비 이미지 설정
	TArray<FGuid> LeaderEquipmentIDs = SaveGameSubsystem->GetAllLeaderEquipmentIDs();
}

void ULRPartyLineupWidget::OnRegroupButtonClicked()
{
	UUIManagerSubsystem* UIManagerSubsystem = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManagerSubsystem->SwitchPageUIByID(EUIID::PARTY);
}

