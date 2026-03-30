// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"

void ULRCharacterEntryWidget::RefreshData()
{
	Super::RefreshData();

	Img_Frame->SetBrushFromTexture(TileData->Frame);
}

void ULRCharacterEntryWidget::OnTileClicked()
{
	SetType(ECollectionType::CHARACTER);

	Super::OnTileClicked();
}
