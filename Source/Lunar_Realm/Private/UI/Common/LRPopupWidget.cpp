// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRPopupWidget.h"
#include "Units/LRControllerBase.h"

void ULRPopupWidget::InitializeUI()
{
	Super::InitializeUI();
	UILayer = EUILayer::POPUP;
}

void ULRPopupWidget::BindToPlayerController(ALRControllerBase* PC)
{
}
