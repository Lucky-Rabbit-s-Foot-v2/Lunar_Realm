// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRSystemWidget.h"

ULRSystemWidget::ULRSystemWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::SYSTEM;
	bIsModal = true;
	SetIsFocusable(true);
}
