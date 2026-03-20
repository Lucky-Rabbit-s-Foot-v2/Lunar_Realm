// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRStarBoxWidget.h"

#include "Engine/Texture2D.h"

#include "Components/Image.h"

void ULRStarBoxWidget::RefreshUI()
{
	Super::RefreshUI();

	if (Img_Star1)
	{
		Img_Star1->SetBrushFromTexture(
			StarMasking & Star1Mask ? StarOnTexture : StarOffTexture
		);
	}

	if (Img_Star2)
	{
		Img_Star2->SetBrushFromTexture(
			StarMasking & Star2Mask ? StarOnTexture : StarOffTexture
		);
	}

	if (Img_Star3)
	{
		Img_Star3->SetBrushFromTexture(
			StarMasking & Star3Mask ? StarOnTexture : StarOffTexture
		);
	}
}

void ULRStarBoxWidget::SetStarMasking(int32 InMasking)
{
	StarMasking = InMasking;
	RefreshUI();
}
