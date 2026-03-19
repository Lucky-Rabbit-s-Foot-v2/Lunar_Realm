// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LREntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void ULREntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	TileData = Cast<ULRTileData>(ListItemObject);
	RefreshData();
}

void ULREntryWidget::RefreshData()
{
	if (TileData && Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(TileData->GetIcon());

		if (TileData->IsLocked())
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
}
