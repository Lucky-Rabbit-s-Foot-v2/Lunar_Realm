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

	SetSelected(false);

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
	if (TileData && Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(TileData->Icon);

		if (TileData->bIsLocked)
		{
			Img_Locked->SetVisibility(ESlateVisibility::Visible);
			Img_Black->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_Locked->SetVisibility(ESlateVisibility::Hidden);
			Img_Black->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	Btn_Selected->SetIsEnabled(!TileData->bIsLocked);
}

void ULREntryWidget::SetSelected(bool bSelected)
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