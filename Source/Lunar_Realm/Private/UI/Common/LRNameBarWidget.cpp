// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRNameBarWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void ULRNameBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULRNameBarWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULRNameBarWidget::SetName(const FText& Name)
{
	if (Txt_Name)
	{
		Txt_Name->SetText(Name);
	}
}