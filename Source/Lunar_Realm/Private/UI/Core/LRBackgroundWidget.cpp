// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRBackgroundWidget.h"

#include "Components/Image.h"

ULRBackgroundWidget::ULRBackgroundWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::BACKGROUND;
	SetIsFocusable(false);
}
