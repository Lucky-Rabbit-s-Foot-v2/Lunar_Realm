// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Data/LRDataStructs.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULREquipEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULREquipEntryWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Selected->OnClicked.AddUniqueDynamic(this, &ULREquipEntryWidget::OnTileClicked);
}

void ULREquipEntryWidget::UnbindProperties()
{
	Btn_Selected->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULREquipEntryWidget::RefreshData()
{
	Super::RefreshData();


	Img_Frame->SetOpacity(0.f);
	if (TileData->IsLocked())
	{
		Img_Locked->SetVisibility(ESlateVisibility::Visible);
		Btn_Selected->SetIsEnabled(false);
	}
	else
	{
		Img_Locked->SetVisibility(ESlateVisibility::Hidden);
		Btn_Selected->SetIsEnabled(true);
	}
}

void ULREquipEntryWidget::OnTileClicked()
{
	OnEquipTileClickedDel.Broadcast(TileData->GetID());
}
