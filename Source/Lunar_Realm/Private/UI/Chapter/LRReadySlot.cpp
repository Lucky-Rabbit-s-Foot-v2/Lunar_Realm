// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRReadySlot.h"

#include "Components/Image.h"

void ULRReadySlot::SetData(UTexture2D* InPortrait, UTexture2D* InGrade, UTexture2D* InEquip)
{
	if (Portrait)
	{
		Portrait->SetBrushFromTexture(InPortrait);
	}
	
	if (Grade)
	{
		Grade->SetVisibility(InGrade ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		Grade->SetBrushFromTexture(InGrade);
	}

	if (Equip)
	{
		Equip->SetBrushFromTexture(InEquip);
	}
}