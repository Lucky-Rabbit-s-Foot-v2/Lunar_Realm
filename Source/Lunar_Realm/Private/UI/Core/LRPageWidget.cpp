// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRPageWidget.h"

ULRPageWidget::ULRPageWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::PAGE;
	bIsModal = true;
	SetIsFocusable(true);
}
