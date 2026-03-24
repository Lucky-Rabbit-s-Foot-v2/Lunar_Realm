// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCharacterCard.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/GameDataSubsystem.h"

void ULRCharacterCard::SetCharacterID(const FName& InID)
{
	ID = InID;
	RefreshUI();
}

void ULRCharacterCard::RefreshUI()
{
	Super::RefreshUI();

	if (ID.IsNone())
	{
		SetEmptyCase(true);
		return;
	}

	SetEmptyCase(false);
	UpdateCharacterData();
}

void ULRCharacterCard::UpdateCharacterData()
{
	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		FCharacterStaticData CharacterData = GameDataSubsystem->GetCharacterStaticData(ID);

		Portrait->SetBrushFromTexture(CharacterData.PortraitIcon.LoadSynchronous());
		Grade->SetBrushFromTexture(CharacterData.GradeImage.LoadSynchronous());
		Name->SetText(FText::FromString(CharacterData.CharacterName));
	}
}

void ULRCharacterCard::SetEmptyCase(bool bIsEmpty)
{
	ESlateVisibility NewVisibility = bIsEmpty ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	Portrait->SetVisibility(NewVisibility);
	Grade->SetVisibility(NewVisibility);
	Name->SetVisibility(NewVisibility);
	
	if (EmptyBG && DefaultBG)
	{
		Border->SetBrushFromTexture(bIsEmpty ? EmptyBG : DefaultBG);
	}
}