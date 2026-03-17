// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRBaseWidget.h"
#include "Units/LRControllerBase.h"

ULRBaseWidget::ULRBaseWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::NONE;
	ZOrder = 0;

	bIsOpen = false;
}

void ULRBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindProperties();
	BindSubWidgets();
	BindToController(Cast<ALRControllerBase>(GetOwningPlayer()));
}

void ULRBaseWidget::NativeDestruct()
{
	DeinitializeUI();
	UnbindProperties();
	Super::NativeDestruct();
}

void ULRBaseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	RegisterSubWidgets();
	InitializeUI();
}

void ULRBaseWidget::InitializeUI()
{
	// 자식 클래스에서 오버라이드하여 초기화 로직 구현
	for (auto& SubWidget : SubWidgets)
	{
		if (SubWidget)
		{
			SubWidget->InitializeUI();
		}
	}
}

void ULRBaseWidget::DeinitializeUI()
{
	// 자식 클래스에서 오버라이드하여 정리 로직 구현
	for (auto& SubWidget : SubWidgets)
	{
		if (SubWidget)
		{
			SubWidget->DeinitializeUI();
		}
	}
	SubWidgets.Empty();
}

void ULRBaseWidget::RegisterSubWidgets()
{
	// 자식 클래스에서 오버라이드하여 서브 위젯 등록 로직 구현
	SubWidgets.Empty();
}

void ULRBaseWidget::BindSubWidgets()
{
	// 자식 클래스에서 오버라이드하여 서브 위젯 등록 로직 구현
}

void ULRBaseWidget::BindProperties()
{
	// 자식 클래스에서 오버라이드하여 프로퍼티 바인딩 로직 구현
}

void ULRBaseWidget::UnbindProperties()
{
	// 자식 클래스에서 오버라이드하여 프로퍼티 언바인딩 로직 구현
}

void ULRBaseWidget::BindToController(ALRControllerBase* Controller)
{
	// 자식 클래스에서 오버라이드하여 프로퍼티 언바인딩 로직 구현
}

void ULRBaseWidget::OpenUI()
{
	// 자식 클래스에서 오버라이드하여 추가 로직 구현
	bIsOpen = true;
	SetVisibility(ESlateVisibility::Visible);
	
	RefreshUI();
}

void ULRBaseWidget::CloseUI()
{
	// 자식 클래스에서 오버라이드하여 추가 로직 구현
	bIsOpen = false;
	SetVisibility(ESlateVisibility::Collapsed); 
	// Hidden -> Collapsed : Hidden 은 연산 진행 / Collapsed 는 연산 X
}

void ULRBaseWidget::RefreshUI()
{
	// 자식 클래스에서 오버라이드하여 데이터 갱신 로직 구현
	for (auto& SubWidget : SubWidgets)
	{
		if (SubWidget)
		{
			SubWidget->RefreshUI();
		}
	}
}

void ULRBaseWidget::OnFocusGained()
{
	// 자식 클래스에서 오버라이드 (예: 애니메이션 재생, 입력 활성화 등)
	for (auto& SubWidget : SubWidgets)
	{
		if (SubWidget)
		{
			SubWidget->OnFocusGained();
		}
	}

	RefreshUI();
}

void ULRBaseWidget::OnFocusLost()
{
	// 자식 클래스에서 오버라이드 (예: 애니메이션 정지, 입력 비활성화 등)
	for (auto& SubWidget : SubWidgets)
	{
		if (SubWidget)
		{
			SubWidget->OnFocusLost();
		}
	}
}

void ULRBaseWidget::OnCloseRequested()
{
	if (OnCloseUIRequestedDel.IsBound())
	{
		OnCloseUIRequestedDel.Broadcast(this);
	}
}
