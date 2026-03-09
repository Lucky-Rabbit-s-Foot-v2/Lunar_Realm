// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterEntryWidget.h"

#include "Components/Button.h"

#include "Data/LRDataStructs.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

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

void ULRCharacterEntryWidget::OnTileClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->SetSelectedCharacterID(TileData->GetID());
	}
}
