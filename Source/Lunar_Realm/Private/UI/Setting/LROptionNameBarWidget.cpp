// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LROptionNameBarWidget.h"

#include "Engine/GameInstance.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"

void ULROptionNameBarWidget::NativePreConstruct()
{
	Txt_Name->SetText(OptionName);
}

void ULROptionNameBarWidget::BindProperties()
{
	Super::BindProperties();

	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OnOptionBarChangedDel.AddUniqueDynamic(OptionManager, &UOptionManagerSubsystem::UpdateOptionValue);
	}
}

void ULROptionNameBarWidget::UnbindProperties()
{
	OnOptionBarChangedDel.Clear();
	Super::UnbindProperties();
}


FReply ULROptionNameBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = true;
		UpdateValueFromMouse(InGeometry, InMouseEvent);

		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply ULROptionNameBarWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
	{
		bIsDragging = false;

		int32 Range = FMath::Clamp(MaxValue - MinValue, 0, MaxValue);

		CurrentValue = MinValue + Range * Bar_Progress->GetPercent();

		if (OnOptionBarChangedDel.IsBound())
		{
			OnOptionBarChangedDel.Broadcast(SettingType, CurrentValue);
		}

		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

FReply ULROptionNameBarWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDragging)
	{
		UpdateValueFromMouse(InGeometry, InMouseEvent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void ULROptionNameBarWidget::UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!Bar_Progress) return;

	FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	FVector2D WidgetSize = MyGeometry.GetLocalSize();

	float NewPercent = FMath::Clamp(LocalPos.X / WidgetSize.X, 0.0f, 1.0f);

	Bar_Progress->SetPercent(NewPercent);
}
