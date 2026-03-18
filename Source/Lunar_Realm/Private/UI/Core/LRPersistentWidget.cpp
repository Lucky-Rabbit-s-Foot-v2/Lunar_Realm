// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRPersistentWidget.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Core/LRPopupWidget.h"

ULRPersistentWidget::ULRPersistentWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::PERSISTENT;
}

void ULRPersistentWidget::OpenUI()
{
	Super::OpenUI();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
