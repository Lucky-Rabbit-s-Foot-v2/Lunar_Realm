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
		OnCloseUIRequestedDel.AddDynamic(PC, &ALRControllerBase::CloseWidget);
	}
}

void ULRChapterSelectorWidget::OnBackButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
