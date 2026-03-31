// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LREntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/Texture2D.h"

#include "Units/OutGame/LROutGameController.h"


void ULREntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	TileData = Cast<ULRTileData>(ListItemObject);

	RefreshData();
}

void ULREntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		OnTileClickedDel.AddUniqueDynamic(PC, &ALROutGameController::OnSelectedEntryWidget);
	}
}

void ULREntryWidget::BindProperties()
{
	Super::BindProperties();
	
	Btn_Selected->OnClicked.AddUniqueDynamic(this, &ULREntryWidget::OnTileClicked);
}

void ULREntryWidget::UnbindProperties()
{
	Btn_Selected->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULREntryWidget::RefreshData()
{
	if (!TileData)
	{
		return;
	}

	if (Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(TileData->Icon);
	}
	
	if (Img_Locked && Img_Black)
	{
		ESlateVisibility LockedVisibility = TileData->bIsLocked ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		Img_Locked->SetVisibility(LockedVisibility);
		Img_Black->SetVisibility(LockedVisibility);
		Btn_Selected->SetIsEnabled(!TileData->bIsLocked);
	}

	if (Img_Selected)
	{
		Img_Selected->SetVisibility(TileData->bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void ULREntryWidget::SetSelected(bool bSelected)
{
	if (TileData)
	{
		TileData->bIsSelected = bSelected;
		if (Img_Selected)
		{
			Img_Selected->SetVisibility(TileData->bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		}
	}
}

void ULREntryWidget::OnTileClicked()
{
	SetSelected(true);

	OnTileClickedDel.Broadcast(this);
}

void ULREntryWidget::SetType(ECollectionType InType)
{
	if (TileData)
	{
		TileData->Type = InType;
	}
}