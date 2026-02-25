// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/UIManagerSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUIManagerSubsystem::Deinitialize()
{
	ResetAllUIStates();

	Super::Deinitialize();
}

int32 UUIManagerSubsystem::CalculateZOrder(ULRBaseWidget* Widget) const
{
	if (!Widget)
	{
		return 0;
	}
    
	int32 DefaultZOrder = Widget->ZOrder;
	switch (Widget->UILayer)
	{
		case EUILayer::BACKGROUND:	return DefaultZOrder;
		case EUILayer::PAGE:		return 25 + DefaultZOrder;
		case EUILayer::PERSISTENT:	return 50 + DefaultZOrder;
		case EUILayer::POPUP:		return 100 + PopupUIStack.Num();
		case EUILayer::TOOLTIP:		return 500 + DefaultZOrder;
		case EUILayer::SYSTEM:		return 900 + PopupUIStack.Num();
		default:					return DefaultZOrder;
	}
}

void UUIManagerSubsystem::NotifyInputModeChange()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}
	
	ULRBaseWidget* TopModalWidget = FindTopModalPopup();
	if (TopModalWidget)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetWidgetToFocus(TopModalWidget->TakeWidget());
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
	else
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
}

void UUIManagerSubsystem::CloseUIInternal(ULRBaseWidget* Widget)
{
	if (!Widget || !Widget->IsOpen())
	{
		return;
	}
    
	EUILayer Layer = Widget->UILayer;
	if (Layer == EUILayer::POPUP || Layer == EUILayer::SYSTEM)
	{
		int32 Index = PopupUIStack.Find(Widget);
		if (Index != INDEX_NONE)
		{
			PopupUIStack.RemoveAt(Index);
		}
		Widget->CloseUI();
		RefreshTopPopupUI();
		NotifyInputModeChange();
	}
	else
	{
		Widget->CloseUI();
		PersistentUIMap.Remove(Widget->GetClass());
	}

	Widget->OnCloseUIRequestedDel.RemoveDynamic(this, &UUIManagerSubsystem::CloseUI);
}

void UUIManagerSubsystem::CloseUI(ULRBaseWidget* Widget)
{
	CloseUIInternal(Widget);
}

void UUIManagerSubsystem::CloseTopPopupUI()
{
	if (PopupUIStack.Num() == 0)
	{
		return;
	}
    
	ULRBaseWidget* TopWidget = PopupUIStack.Pop();
	CloseUI(TopWidget);
}

void UUIManagerSubsystem::CloseAllPopupUI()
{
	while (PopupUIStack.Num() > 0)
	{
		CloseTopPopupUI();
	}
}

void UUIManagerSubsystem::RefreshTopPopupUI()
{
	// 스택에 팝업이 남아있다면 새로운 Top에게 포커스 전달
	if (PopupUIStack.Num() > 0)
	{
		PopupUIStack.Last()->OnFocusGained();
	}
}

void UUIManagerSubsystem::ResetAllUIStates()
{
	//캐싱된 인스턴스들 상태 초기화
	for (auto& pair : CachedWidgets)
	{
		ResetUIState(pair.Value);
	}
	
	//관련 컨테이너 비우기
	PersistentUIMap.Empty();
	PopupUIStack.Empty();
	CachedWidgets.Empty ();
}

void UUIManagerSubsystem::UpdatePopupZOrders()
{
	for (int32 i = 0; i < PopupUIStack.Num(); ++i)
	{
		ULRBaseWidget* Widget = PopupUIStack[i];
		if (Widget && Widget->IsInViewport())
		{
			int32 NewZOrder = CalculateZOrder(Widget);
			Widget->AddToViewport(NewZOrder + i);
		}
	}
}

ULRBaseWidget* UUIManagerSubsystem::FindTopModalPopup()
{
	for (int32 i = PopupUIStack.Num() - 1; i >= 0; --i)
	{
		if (PopupUIStack[i]->bIsModal)
		{
			return PopupUIStack[i];
		}
	}
	return nullptr;
}

void UUIManagerSubsystem::ResetUIState(ULRBaseWidget* Widget)
{
	if (Widget)
	{
		if (Widget->IsOpen())
		{
			Widget->CloseUI();
		}
		if (Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}
	}
}
