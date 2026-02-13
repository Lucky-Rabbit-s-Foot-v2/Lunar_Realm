// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterSelectorWidget.h"

#include "Components/Button.h"

#include "Units/LRControllerBase.h"

void ULRChapterSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Back)
	{
		Btn_Back->OnClicked.AddDynamic(this, &ULRChapterSelectorWidget::OnBackButtonClicked);
	}

	if (ALRControllerBase* PC = Cast<ALRControllerBase>(GetWorld()->GetFirstPlayerController()))
	{
		OnCloseUIRequested.AddDynamic(PC, &ALRControllerBase::CloseWidget);
	}
}

void ULRChapterSelectorWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRChapterSelectorWidget::OpenUI()
{
	Super::OpenUI();
}

void ULRChapterSelectorWidget::CloseUI()
{
	Super::CloseUI();
}

void ULRChapterSelectorWidget::RefreshUI()
{
	Super::RefreshUI();
}

void ULRChapterSelectorWidget::OnBackButtonClicked()
{
	OnCloseUIRequested.Broadcast(this);
}
