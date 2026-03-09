// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRPopupWidget.h"
#include "Units/LRControllerBase.h"

ULRPopupWidget::ULRPopupWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::POPUP;
}
