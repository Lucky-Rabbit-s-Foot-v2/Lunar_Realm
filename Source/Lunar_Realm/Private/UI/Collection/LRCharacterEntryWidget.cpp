// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Data/LRDataStructs.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULRCharacterEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetSelected(false);

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		OnTileClickedDel.AddUniqueDynamic(PC, &ALROutGameController::SetSelectedCharacterID);
	}
}

void ULRCharacterEntryWidget::BindProperties()
{
	Super::BindProperties();

	Btn_Selected->OnClicked.AddDynamic(this, &ULRCharacterEntryWidget::OnTileClicked);
}

void ULRCharacterEntryWidget::UnbindProperties()
{
	Btn_Selected->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRCharacterEntryWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);
	
	ALROutGameController* PC = Cast<ALROutGameController>(Controller);
	if (PC)
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULRCharacterEntryWidget::IsSelectedTile);
	}
}

void ULRCharacterEntryWidget::RefreshData()
{
	Super::RefreshData();

	Img_Frame->SetBrushFromTexture(TileData->GetFrame());

	Btn_Selected->SetIsEnabled(!TileData->IsLocked());
	
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		FName SelectedID = PC->GetSelectedCharacterID();
		bool bIsSelectedNow = (SelectedID == TileData->GetID()) && (SelectedID != NAME_None);
		SetSelected(bIsSelectedNow);
	}
}

void ULRCharacterEntryWidget::IsSelectedTile(ESelectedType SelectedType, FName SelectedID)
{
	if (SelectedType != ESelectedType::CHARACTER)
	{
		SetSelected(false);
		return;
	}
	SetSelected(TileData->GetID() == SelectedID);
}

void ULRCharacterEntryWidget::OnTileClicked()
{
	SetSelected(true);

	OnTileClickedDel.Broadcast(TileData->GetID());
}

void ULRCharacterEntryWidget::SetSelected(bool bSelected)
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