// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRPopupWidget.h"
#include "Units/LRControllerBase.h"

ULRPopupWidget::ULRPopupWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::POPUP;
	ZOrder = 100;
	bIsModal = true;
	bIsFocusable = true;
}

FReply ULRPopupWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::Android_Back)
	{
		OnCloseRequested();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void ULRPopupWidget::InitializeUI()
{
	Super::InitializeUI();
	UILayer = EUILayer::POPUP;
}

void ULRPopupWidget::BindToPlayerController(ALRControllerBase* PC)
{
}
