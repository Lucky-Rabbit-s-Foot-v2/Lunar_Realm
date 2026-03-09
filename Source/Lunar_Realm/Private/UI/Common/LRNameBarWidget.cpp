// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRNameBarWidget.h"

#include "Engine/GameInstance.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"

void ULRNameBarWidget::BindProperties()
{
	Super::BindProperties();

	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OnProgressBarChangedDel.AddUniqueDynamic(OptionManager, &UOptionManagerSubsystem::UpdateOptionValue);
	}
}

void ULRNameBarWidget::UnbindProperties()
{
	OnProgressBarChangedDel.Clear();

	Super::UnbindProperties();
}

void ULRNameBarWidget::SetName(const FText& Name)
{
	if (Txt_Name)
	{
		Txt_Name->SetText(Name);
	}
}

FReply ULRNameBarWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = true;
		UpdateValueFromMouse(InGeometry, InMouseEvent);

		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply ULRNameBarWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
	{
		bIsDragging = false;

		int32 Range = FMath::Clamp(MaxValue - MinValue, 0, MaxValue);

		CurrentValue = MinValue + Range * Bar_Progress->GetPercent();

		if (OnProgressBarChangedDel.IsBound())
		{
			OnProgressBarChangedDel.Broadcast(SettingType, CurrentValue);
		}

		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

FReply ULRNameBarWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDragging)
	{
		UpdateValueFromMouse(InGeometry, InMouseEvent);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void ULRNameBarWidget::UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!Bar_Progress) return;

	FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	FVector2D WidgetSize = MyGeometry.GetLocalSize();

	float NewPercent = FMath::Clamp(LocalPos.X / WidgetSize.X, 0.0f, 1.0f);

	Bar_Progress->SetPercent(NewPercent);
}
