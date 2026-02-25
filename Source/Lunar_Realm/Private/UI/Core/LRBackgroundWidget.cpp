// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRBackgroundWidget.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

ULRBackgroundWidget::ULRBackgroundWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::BACKGROUND;
	bDisableWorldRenderingWhenOpened = false;
}
