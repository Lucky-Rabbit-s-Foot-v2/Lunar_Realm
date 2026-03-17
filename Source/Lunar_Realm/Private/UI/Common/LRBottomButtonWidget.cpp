// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRBottomButtonWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Core/LRPageWidget.h"

void ULRBottomButtonWidget::BindProperties()
{
	Super::BindProperties();

	if (Img_Background)
	{
		Img_Background->OnMouseButtonDownEvent.BindDynamic(this, &ULRBottomButtonWidget::OnBackgroundMouseButtonDown);
	}
	else
	{
		LR_WARN("Img_Background is null. Cannot bind mouse button down event.");
	}
}

void ULRBottomButtonWidget::UnbindProperties()
{
	if (Img_Background)
	{
		Img_Background->OnMouseButtonDownEvent.Unbind();
	}
	else
	{
		LR_WARN("Img_Background is null. Cannot unbind mouse button down event.");
	}

	Super::UnbindProperties();
}

FEventReply ULRBottomButtonWidget::OnBackgroundMouseButtonDown(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(MovePageUIID);
	}
	else
	{
		LR_WARN("UIManagerSubsystem not found in GameInstance.");
	}

	return UWidgetBlueprintLibrary::Handled();
}
