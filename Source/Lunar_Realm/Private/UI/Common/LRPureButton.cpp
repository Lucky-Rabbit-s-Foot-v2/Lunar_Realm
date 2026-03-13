// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRPureButton.h"

ULRPureButton::ULRPureButton()
{
	ApplyTransparentStyle();
}

void ULRPureButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	ApplyTransparentStyle();
}

void ULRPureButton::ApplyTransparentStyle()
{
	FButtonStyle TransparentStyle = GetStyle();

	FSlateBrush TransparentBrush;
	TransparentBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
	TransparentBrush.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f));

	TransparentStyle.SetNormal(TransparentBrush);
	TransparentStyle.SetHovered(TransparentBrush);
	TransparentStyle.SetPressed(TransparentBrush);

	FMargin NoPadding(0.f);
	TransparentStyle.SetNormalPadding(NoPadding);
	TransparentStyle.SetPressedPadding(NoPadding);

	SetStyle(TransparentStyle);
	SetBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.f));
}
