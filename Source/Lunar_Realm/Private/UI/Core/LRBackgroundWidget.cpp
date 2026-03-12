// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRBackgroundWidget.h"

#include "Engine/Texture2D.h"

#include "Components/Image.h"
#include "Components/Button.h"

ULRBackgroundWidget::ULRBackgroundWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::BACKGROUND;
}
