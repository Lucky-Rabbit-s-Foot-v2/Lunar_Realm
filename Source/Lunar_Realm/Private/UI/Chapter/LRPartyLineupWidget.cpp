// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRPartyLineupWidget.h"

#include "Components/Image.h"
#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Subsystems/SaveGameSubsystem.h"

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
	TArray<FName> PartyCharacterIDs = SaveGameSubsystem->GetAllPartyCharactersIDs();
	TArray<FGuid> LeaderEquipmentIDs = SaveGameSubsystem->GetAllLeaderEquipmentIDs();

	// TODO: 각 캐릭터 / 장비 이미지 설정

}

void ULRPartyLineupWidget::OnRegroupButtonClicked()
{
	UUIManagerSubsystem* UIManagerSubsystem = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManagerSubsystem->SwitchPageUIByID(EUIID::PARTY);
}

