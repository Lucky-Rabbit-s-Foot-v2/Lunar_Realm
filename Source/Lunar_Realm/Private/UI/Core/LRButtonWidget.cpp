// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "TimerManager.h"

void ULRButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text)
	{
		Text->SetText(ButtonText);
	}
}

void ULRButtonWidget::BindProperties()
{
	Super::BindProperties();
	if (Button)
	{
		Button->OnClicked.AddUniqueDynamic(this, &ULRButtonWidget::OnButtonClicked);
	}
}

void ULRButtonWidget::OnButtonClicked()
{
	if (!bCanClicked)
	{
		return;
	}

	bCanClicked = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ClickTimer);
		
		TWeakObjectPtr<ULRButtonWidget> WeakThis(this);
		World->GetTimerManager().SetTimer(
			ClickTimer,
			[WeakThis]
			{
				WeakThis->bCanClicked = true;
			},
			0.1f,
			false
		);
	}

	if (OnLRButtonClickedDel.IsBound())
	{
		OnLRButtonClickedDel.Broadcast();
	}
}