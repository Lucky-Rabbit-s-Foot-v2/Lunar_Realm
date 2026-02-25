// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageSelectorWidget.h"

#include "Components/Button.h"
#include "UI/Chapter/LRStageWidget.h"

#include "Units/LRControllerBase.h"

void ULRStageSelectorWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRStageSelectorWidget::OnBackButtonClicked);
}

void ULRStageSelectorWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRStageSelectorWidget::OnBackButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
