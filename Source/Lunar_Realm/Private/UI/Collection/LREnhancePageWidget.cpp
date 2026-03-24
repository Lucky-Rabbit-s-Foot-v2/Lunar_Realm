// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhancePageWidget.h"

#include "Engine/GameInstance.h"

#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterEnhanceWidget.h"

void ULREnhancePageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterEnhance);
	SubWidgets.Add(Collection);
}

void ULREnhancePageWidget::InitializeUI()
{
	Super::InitializeUI();
	if (CharacterID.IsNone())
	{
		USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
		if (SaveGameSubsystem)
		{
			FName LeaderID = SaveGameSubsystem->GetLeaderCharacterID();
			SetCharacterID(LeaderID);
		}
	}
}

void ULREnhancePageWidget::SetCharacterID(FName InID)
{
	CharacterID = InID;
	CharacterEnhance->SetCharacterID(CharacterID);
	RefreshUI();
}
