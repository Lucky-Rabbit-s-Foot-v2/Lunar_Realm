// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRBaseWidget.h"
#include "Units/LRControllerBase.h"

ULRBaseWidget::ULRBaseWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UILayer = EUILayer::NONE;
	ZOrder = 0;

	bIsOpen = false;
	bIsModal = false;
	bIsFocusable = false;
}

void ULRBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindToController(Cast<ALRControllerBase>(GetOwningPlayer()));
	InitializeUI();
}

void ULRBaseWidget::NativeDestruct()
{
	// 공용 델리게이트 해제
	if (OnCloseUIRequested.IsBound())
	{
		OnCloseUIRequested.Clear();
	}
	Super::NativeDestruct();
}

void ULRBaseWidget::InitializeUI()
{
	// 자식 클래스에서 오버라이드하여 초기화 로직 구현
}

void ULRBaseWidget::OpenUI()
{
	RefreshUI();

	bIsOpen = true;
	SetVisibility(ESlateVisibility::Visible);
	// 자식 클래스에서 오버라이드하여 추가 로직 구현
}

void ULRBaseWidget::CloseUI()
{
	bIsOpen = false;
	// Hidden ->Collapsed : Hidden 은 연산 진행 / Collapsed 는 연산 X
	SetVisibility(ESlateVisibility::Collapsed);
	// 자식 클래스에서 오버라이드하여 추가 로직 구현
}

void ULRBaseWidget::RefreshUI()
{
	// 자식 클래스에서 오버라이드하여 데이터 갱신 로직 구현
}

void ULRBaseWidget::OnFocusGained()
{
	// 자식 클래스에서 오버라이드 (예: 애니메이션 재생, 입력 활성화 등)
}

void ULRBaseWidget::OnFocusLost()
{
	// 자식 클래스에서 오버라이드 (예: 애니메이션 정지, 입력 비활성화 등)
}

void ULRBaseWidget::BindToController(ALRControllerBase* Controller)
{
	// 자식 클래스에서 오버라이드하여 컨트롤러와의 바인딩 로직 구현
}

void ULRBaseWidget::OnCloseRequested()
{
	if (OnCloseUIRequested.IsBound())
	{
		OnCloseUIRequested.Broadcast(this);
	}
}
