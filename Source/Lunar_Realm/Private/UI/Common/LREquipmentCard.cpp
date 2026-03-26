// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LREquipmentCard.h"


#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/GameDataSubsystem.h"

void ULREquipmentCard::SetEquipID(const FName& InID)
{
	ID = InID;
	RefreshUI();
}

void ULREquipmentCard::RefreshUI()
{
	Super::RefreshUI();

	if (ID.IsNone())
	{
		SetEmptyCase(true);
		return;
	}

	SetEmptyCase(false);
	UpdateEquipData();
}

void ULREquipmentCard::UpdateEquipData()
{
	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		FEquipmentStaticData EquipmentData = GameDataSubsystem->GetEquipmentStaticData(ID);
		Portrait->SetBrushFromTexture(EquipmentData.EquipmentTexture.LoadSynchronous());

		// TODO: 등급 이미지 추가 필요
		Grade->SetVisibility(ESlateVisibility::Hidden);
		//Grade->SetBrushFromTexture(EquipmentData.GradeImage.LoadSynchronous());

		Name->SetText(FText::FromString(EquipmentData.EquipmentName));
	}
}

void ULREquipmentCard::SetEmptyCase(bool bIsEmpty)
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