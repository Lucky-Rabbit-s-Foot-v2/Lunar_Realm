// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageSelectorWidget.h"

#include "Components/Button.h"
#include "UI/Chapter/StageWidget.h"

#include "Units/LRControllerBase.h"

void ULRStageSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &ULRStageSelectorWidget::OnBackButtonClicked);
	}

	if(ALRControllerBase* PC = Cast<ALRControllerBase>(GetWorld()->GetFirstPlayerController()))
	{
		OnCloseUIRequested.AddDynamic(PC, &ALRControllerBase::CloseWidget);
	}
}

void ULRStageSelectorWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRStageSelectorWidget::OpenUI()
{
	Super::OpenUI();
}

void ULRStageSelectorWidget::CloseUI()
{
	Super::CloseUI();
}

void ULRStageSelectorWidget::RefreshUI()
{
	Super::RefreshUI();
}

void ULRStageSelectorWidget::OnBackButtonClicked()
{
	OnCloseUIRequested.Broadcast(this);
}
