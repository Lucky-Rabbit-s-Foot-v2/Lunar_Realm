// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRToolTipWidget.h"

ULRToolTipWidget::ULRToolTipWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::TOOLTIP;
}
