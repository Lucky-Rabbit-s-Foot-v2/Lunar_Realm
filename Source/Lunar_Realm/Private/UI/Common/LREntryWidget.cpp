// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LREntryWidget.h"

#include "Components/Image.h"

void ULREntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	
}

void ULREntryWidget::SetData(const FString& InID, UImage* InIcon)
{
	ID = InID;
	
	if (Img_Icon && InIcon)
	{
		// TODO : 이미지 갱신
	}
}
