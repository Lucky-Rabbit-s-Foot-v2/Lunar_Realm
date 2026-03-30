// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyFigureInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

void ULRLobbyFigureInfoWidget::RefreshUI()
{
	Super::RefreshUI();

	if (SlotIndex < 0)
	{
		Img_Equipment->SetVisibility(ESlateVisibility::Hidden);
		Txt_Name->SetText(FText::GetEmpty());
		return;
	}
	
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	FGuid EquipmentGuid = SaveGameSubsystem->GetLeaderEquipmentID(SlotIndex);

	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	const FEquipmentInstance& Instances = CollectionSubsystem->GetEquipmentInstance(EquipmentGuid);
	FName EquipmentID = Instances.EquipmentID;

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const FEquipmentStaticData& StaticData = GameDataSubsystem->GetEquipmentStaticData(EquipmentID);
	
	UTexture2D* EquipmentIcon = StaticData.EquipmentTexture.LoadSynchronous();
	if (EquipmentIcon)
	{
		SetVisibility(ESlateVisibility::Visible);
		Img_Equipment->SetBrushFromTexture(EquipmentIcon);
		Txt_Name->SetText(FText::FromString(StaticData.EquipmentName));
	}
	else
	{
		Img_Equipment->SetVisibility(ESlateVisibility::Hidden);
		Txt_Name->SetText(FText::GetEmpty());
	}
}

void ULRLobbyFigureInfoWidget::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	RefreshUI();
}