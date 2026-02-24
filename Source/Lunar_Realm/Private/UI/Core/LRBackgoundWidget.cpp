// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRBackgoundWidget.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

ULRBackgoundWidget::ULRBackgoundWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::BACKGROUND;
	ZOrder = 0;
	bDisableWorldRenderingWhenOpened = false;
}
