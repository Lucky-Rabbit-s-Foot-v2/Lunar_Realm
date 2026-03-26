// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/UIManagerSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/Core/LRDamageWidget.h"

#include "UI/Core/LRBackgroundWidget.h"
#include "UI/Common/LRTouchEffectWidget.h"

#include "Subsystems/Settings/UIManagerSettings.h"

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
		return -1;
	}
    
	int32 DefaultZOrder = Widget->ZOrder;
	switch (Widget->UILayer)
	{
		case EUILayer::BACKGROUND:	return DefaultZOrder;
		case EUILayer::PAGE:		return 10 + DefaultZOrder;
		case EUILayer::PERSISTENT:	return 100 + DefaultZOrder;
		case EUILayer::POPUP:		return 200 + TransientUIStack.Num();
		case EUILayer::OVERLAY:		return 500 + DefaultZOrder;
		case EUILayer::TOOLTIP:		return 600 + DefaultZOrder;
		case EUILayer::SYSTEM:		return 900 + DefaultZOrder;
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
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetWidgetToFocus(nullptr);
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);

	/* 게임 입력 모드 변경
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);
	*/
}

void UUIManagerSubsystem::CloseUIInternal(ULRBaseWidget* Widget)
{
	if (!Widget || !Widget->IsOpen())
	{
		return;
	}

	EUILayer Layer = Widget->UILayer;
	switch (Layer)
	{
		case EUILayer::PAGE:
		{
			if (PageWidget == Widget)
			{
				PageWidget->CloseUI();
				PageWidget = nullptr;
				CloseAllPopupUI();
			}
			break;
		}
		case EUILayer::POPUP:
		case EUILayer::SYSTEM:		// 시스템 UI도 팝업처럼 스택에서 관리
		{
			int32 Index = TransientUIStack.Find(Widget);
			if (Index != INDEX_NONE)
			{
				while (TransientUIStack.Num() > Index)
				{
					ULRBaseWidget* TopWidget = TransientUIStack.Pop();
					TopWidget->CloseUI();

					UIHistoryStack.Pop();
				}
			}
			RefreshTopPopupUI();
			NotifyInputModeChange();
			break;
		}
		case EUILayer::BACKGROUND:
		{
			if (BackgroundWidget == Widget)
			{
				Widget->CloseUI();
				BackgroundWidget = nullptr;
			}
			break;
		}
		case EUILayer::PERSISTENT:
		{
			if (PermenentUIMap.Contains(Widget->GetClass()) && PermenentUIMap[Widget->GetClass()] == Widget)
			{
				Widget->CloseUI();
				PermenentUIMap.Remove(Widget->GetClass());
			}
			break;
		}
		default: // NONE, TOOLTIP, OVERLAY 등은 특별한 관리 없이 닫기만 하면 됨
		{
			break;
		}
	}
	return;
}

void UUIManagerSubsystem::CloseUI(ULRBaseWidget* Widget)
{
	CloseUIInternal(Widget);
}

void UUIManagerSubsystem::CloseTopPopupUI()
{
	if (TransientUIStack.Num() == 0)
	{
		return;
	}
	ULRBaseWidget* TopWidget = TransientUIStack.Last();
	CloseUI(TopWidget);
}

void UUIManagerSubsystem::CloseAllPopupUI()
{
	while (TransientUIStack.Num() > 0)
	{
		ULRBaseWidget* TopWidget = TransientUIStack.Pop();
		TopWidget->CloseUI();

		// 모든 팝이 닫히면서 아래 팝업이 Refresh 되므로 비효율적
		//CloseTopPopupUI();
	}
}

void UUIManagerSubsystem::RefreshTopPopupUI()
{
	// 스택에 팝업이 남아있다면 새로운 Top에게 포커스 전달
	if (TransientUIStack.Num() > 0)
	{
		TransientUIStack.Last()->OnFocusGained();
	}
}

void UUIManagerSubsystem::ResetAllUIStates()
{
	//캐싱된 인스턴스들 상태 초기화
	for (auto& pair : CachedWidgets)
	{
		ResetUIState(pair.Value);
		pair.Value = nullptr;
	}
	
	//관련 컨테이너 비우기
	UIHistoryStack.Empty();
	PermenentUIMap.Empty();
	TransientUIStack.Empty();
	CachedWidgets.Empty ();
}

void UUIManagerSubsystem::DoUIHistory(ULRBaseWidget* Widget)
{
	if (UIHistoryStack.Num() > 0 && UIHistoryStack.Last() == Widget)
	{
		return;
	}
	UIHistoryStack.Add(Widget);
	OnHistoryChangedDel.Broadcast();
}

void UUIManagerSubsystem::UndoUIHistory()
{
	if (ULRBaseWidget* CurrentWidget = UIHistoryStack.Num() > 1 ? UIHistoryStack.Pop() : nullptr)
	{
		CloseUI(CurrentWidget);
	}
	if (ULRBaseWidget* PreviousWidget = UIHistoryStack.Num() > 0 ? UIHistoryStack.Last() : nullptr)
	{
		PreviousWidget->OpenUI();
	}
	OnHistoryChangedDel.Broadcast();
}

void UUIManagerSubsystem::ClearUIHistory()
{
	UIHistoryStack.Empty();
	OnHistoryChangedDel.Broadcast();
}

ULRBaseWidget* UUIManagerSubsystem::GetTopUIInHistory() const
{
	return UIHistoryStack.Num() > 0 ? UIHistoryStack.Last() : nullptr;
}

ULRBaseWidget* UUIManagerSubsystem::OpenUIByID(EUIID UIID)
{
	const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
	if (!Settings)
	{
		return nullptr;
	}

	if (const TSoftClassPtr<ULRBaseWidget>* SoftClassPtr = Settings->UIClassMap.Find(UIID))
	{
		UClass* LoadedClass = SoftClassPtr->LoadSynchronous();
		if (LoadedClass)
		{
			return OpenUI<ULRBaseWidget>(LoadedClass);
		}
		else
		{
			LR_INFO(TEXT("Failed to load widget class for PageID %d"), static_cast<uint8>(UIID));
		}
	}
	else
	{
		LR_INFO(TEXT("PageID %d not found in UIManagerSettings"), static_cast<uint8>(UIID));
	}

	return nullptr;
}

ULRBaseWidget* UUIManagerSubsystem::GetOrCreateWidgetByID(EUIID UIID)
{
	const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
	if (const TSoftClassPtr<ULRBaseWidget>* SoftClassPtr = Settings->UIClassMap.Find(UIID))
	{
		UClass* LoadedClass = SoftClassPtr->LoadSynchronous();
		if (LoadedClass)
		{
			return GetOrCreateWidget<ULRBaseWidget>(LoadedClass);
		}
		else
		{
			LR_INFO(TEXT("Failed to load widget class for PageID %d"), static_cast<uint8>(UIID));
		}
	}
	else
	{
		LR_INFO(TEXT("PageID %d not found in UIManagerSettings"), static_cast<uint8>(UIID));
	}

	return nullptr;
}

void UUIManagerSubsystem::ShowBackgroundUI()
{
	if (!BackgroundWidget)
	{
		BackgroundWidget = OpenUIByID(EUIID::BACKGROUND);
	}
	BackgroundWidget->OpenUI();
}

void UUIManagerSubsystem::HideBackgroundUI()
{
	if (!BackgroundWidget)
	{
		BackgroundWidget = OpenUIByID(EUIID::BACKGROUND);
	}
	BackgroundWidget->CloseUI();
}

void UUIManagerSubsystem::ShowDamageText(float Damage, FVector HitLocation, FLinearColor InColor)
{
	ULRDamageWidget* DamageUI = GetFreeDamageWidgetFromPool();
    if (!DamageUI) return;

	DamageUI->ActivateWidget();

	DamageUI->SetDamageColor(InColor);

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    FVector2D ScreenPosition;
    
    // 3D 월드 좌표(HitLocation)를 2D 스크린 좌표(ScreenPosition)로 전환
    if (UGameplayStatics::ProjectWorldToScreen(PC, HitLocation, ScreenPosition))
    {
		DamageUI->SetPositionInViewport(ScreenPosition);

		//==================START:스케일링 로직====================
        FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
        float Distance = FVector::Distance(CameraLocation, HitLocation);

        // 거리에 따른 스케일(비율) 매핑
        FVector2D InRange(500.0f, 3000.0f);		// 거리 : 500 ~ 3000 
        FVector2D OutRange(1.0f, 0.4f);			// 스케일 : 1.0f ~ 0.4f 
        float DynamicScale = FMath::GetMappedRangeValueClamped(InRange, OutRange, Distance);

        // 렌더 스케일 적용
        DamageUI->SetRenderScale(FVector2D(DynamicScale, DynamicScale));
		//====================END:스케일링 로직==================
		DamageUI->PlayFloatAnimation(Damage);
    }
}

ULRDamageWidget* UUIManagerSubsystem::GetFreeDamageWidgetFromPool()
{
	ULRDamageWidget* Widget = nullptr;

	if (DamageWidgetPool.Num() > 0)
	{
		Widget = DamageWidgetPool.Pop();
	}
	else
	{
		const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
		Widget = CreateWidget<ULRDamageWidget>(GetWorld(), Settings->DamageWidgetClass);
	}
	return Widget;
}

void UUIManagerSubsystem::ReturnDamageWidgetToPool(ULRDamageWidget* Widget)
{
	if (Widget)
	{
		Widget->DeactivateWidget();
		DamageWidgetPool.Add(Widget);
	}
}

void UUIManagerSubsystem::ShowTouchEffect(const FVector2D& InScreenPosition)
{
	ULRTouchEffectWidget* TouchUI = GetFreeTouchWidgetFromPool();
	if (!TouchUI) return;

	TouchUI->AddToViewport(9999);

	TouchUI->SetPositionInViewport(InScreenPosition);

	TouchUI->PlayRippleAnimation();
}


ULRTouchEffectWidget* UUIManagerSubsystem::GetFreeTouchWidgetFromPool()
{
	ULRTouchEffectWidget* Widget = nullptr;

	if (TouchWidgetPool.Num() > 0)
	{
		Widget = TouchWidgetPool.Pop(); // 풀에 남은 게 있으면 하나 꺼냄
	}
	else
	{
		const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
		if (Settings && Settings->TouchWidgetClass)
		{
			Widget = CreateWidget<ULRTouchEffectWidget>(GetWorld(), Settings->TouchWidgetClass);
		}
	}
	return Widget;
}
void UUIManagerSubsystem::ReturnTouchWidgetToPool(ULRTouchEffectWidget* InWidget)
{
	if (InWidget)
	{
		TouchWidgetPool.Add(InWidget);
	}
}

void UUIManagerSubsystem::InitializeDamageWidgetPool(int32 PoolSize)
{
	const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
	if (!Settings)
	{
		return;
	}
	
	for (int32 i = 0; i < PoolSize; ++i)
	{
		if (ULRDamageWidget* Widget = CreateWidget<ULRDamageWidget>(GetWorld(), Settings->DamageWidgetClass))
		{
			DamageWidgetPool.Add(Widget);
			Widget->CloseUI();
		}
	}
}

void UUIManagerSubsystem::ResetDamageWidgetPool()
{
	for (auto& Widget : DamageWidgetPool)
	{
		if (Widget)
		{
			Widget->RemoveFromParent();
		}
	}
	DamageWidgetPool.Empty();
}

void UUIManagerSubsystem::UpdatePopupZOrders()
{
	for (int32 i = 0; i < TransientUIStack.Num(); ++i)
	{
		ULRBaseWidget* Widget = TransientUIStack[i];
		if (Widget && Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
			int32 NewZOrder = CalculateZOrder(Widget) + i;
			Widget->AddToViewport(NewZOrder);
		}
	}
}

void UUIManagerSubsystem::ResetUIState(ULRBaseWidget* Widget)
{
	if (!Widget)
	{
		return;
	}

	if (Widget->IsOpen())
	{
		Widget->CloseUI();
	}
	
	if (Widget->IsInViewport())
	{
		Widget->RemoveFromParent();
	}
	
	Widget = nullptr;
}
