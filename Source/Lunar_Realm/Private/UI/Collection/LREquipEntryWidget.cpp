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

	SetSelected(false);

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		OnEquipTileClickedDel.AddUniqueDynamic(PC, &ALROutGameController::SetSelectedEquipmentID);
	}
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

void ULREquipEntryWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);
	
	ALROutGameController* PC = Cast<ALROutGameController>(Controller);
	if (PC)
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULREquipEntryWidget::IsSelectedTile);
	}
}

void ULREquipEntryWidget::RefreshData()
{
	Super::RefreshData();

	Btn_Selected->SetIsEnabled(!TileData->IsLocked());

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		FName SelectedID = PC->GetSelectedEquipmentID();
		bool bIsSelectedNow = (SelectedID == TileData->GetID()) && (SelectedID != NAME_None);
		SetSelected(bIsSelectedNow);
	}
}

void ULREquipEntryWidget::IsSelectedTile(ESelectedType SelectedType, FName SelectedID)
{
	if (SelectedType != ESelectedType::EQUIPMENT)
	{
		SetSelected(false);
		return;
	}
	SetSelected(TileData->GetID() == SelectedID);
}

void ULREquipEntryWidget::OnTileClicked()
{
	SetSelected(true);

	OnEquipTileClickedDel.Broadcast(TileData->GetID());
}

void ULREquipEntryWidget::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
	if (bIsSelected)
	{
		Img_Selected->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Img_Selected->SetVisibility(ESlateVisibility::Hidden);
	}
}