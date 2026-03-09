// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/UIManagerSubsystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/InGame/LRDamageWidget.h"

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
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	//InputMode.SetWidgetToFocus(TopModalWidget->TakeWidget());
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
	if (Layer == EUILayer::POPUP || Layer == EUILayer::SYSTEM || Layer == EUILayer::PAGE)
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

ULRBaseWidget* UUIManagerSubsystem::SwitchPageUIByID(EUIID PageID)
{
	const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
	if (!Settings)
	{
		return nullptr;
	}

	if(const TSoftClassPtr<ULRBaseWidget>* SoftClassPtr = Settings->UIClassMap.Find(PageID))
	{
		UClass* LoadedClass = SoftClassPtr->LoadSynchronous();
		if (LoadedClass)
		{
			return SwitchPageUI<ULRBaseWidget>(LoadedClass);
		}
		else
		{
			LR_INFO(TEXT("Failed to load widget class for PageID %d"), static_cast<uint8>(PageID));
		}
	}
	else
	{
		LR_INFO(TEXT("PageID %d not found in UIManagerSettings"), static_cast<uint8>(PageID));
	}

	return nullptr;
}

void UUIManagerSubsystem::ShowDamageText(float Damage, FVector HitLocation, FLinearColor InColor)
{
	ULRDamageWidget* DamageUI = GetFreeDamageWidgetFromPool();
    if (!DamageUI) return;

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
        FVector2D InRange(500.0f, 3000.0f); // 거리 : 500 ~ 3000 
        FVector2D OutRange(1.0f, 0.4f); // 스케일 : 1.0f ~ 0.4f 
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
		UUIManagerSettings* Settings = GetMutableDefault<UUIManagerSettings>();
		Widget = CreateWidget<ULRDamageWidget>(GetWorld(), Settings->DamageWidgetClass);
	}
	
	if (Widget)
	{
		Widget->ActivateWidget();
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

void UUIManagerSubsystem::InitializeDamageWidgetPool(int32 PoolSize)
{
	for (int32 i = 0; i < PoolSize; ++i)
	{
		UUIManagerSettings* Settings = GetMutableDefault<UUIManagerSettings>();
		ULRDamageWidget* Widget = CreateWidget<ULRDamageWidget>(GetWorld(), Settings->DamageWidgetClass);
		if (Widget)
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			DamageWidgetPool.Add(Widget);
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
	for (int32 i = 0; i < PopupUIStack.Num(); ++i)
	{
		ULRBaseWidget* Widget = PopupUIStack[i];
		if (Widget && Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
			int32 NewZOrder = CalculateZOrder(Widget) + i;
			Widget->AddToViewport(NewZOrder);
			LR_INFO(TEXT("Adding %s to viewport with ZOrder %d"), *Widget->GetName(), NewZOrder);
		}
	}
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
