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

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		OnTileClickedDel.AddDynamic(PC, &ALROutGameController::SetSelectedCharacterID);
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

void ULRCharacterEntryWidget::RefreshData()
{
	Super::RefreshData();

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

void ULRCharacterEntryWidget::OnTileClicked()
{
	OnTileClickedDel.Broadcast(TileData->GetID());
}
