// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LROverlayWidget.h"


ULROverlayWidget::ULROverlayWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::OVERLAY;
}

void ULROverlayWidget::OpenUI()
{
	Super::OpenUI();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
