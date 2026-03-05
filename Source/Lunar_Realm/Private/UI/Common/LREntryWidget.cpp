// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LREntryWidget.h"

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
	}
}
