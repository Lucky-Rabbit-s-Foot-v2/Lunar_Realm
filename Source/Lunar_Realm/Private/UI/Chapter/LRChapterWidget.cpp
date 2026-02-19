// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Units/OutGame/LROutGameController.h"

void ULRChapterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Open)
	{
		Btn_Open->OnClicked.AddDynamic(this, &ULRChapterWidget::OnOpenButtonClicked);
	}

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetWorld()->GetFirstPlayerController()))
	{
		OnChapterOpenClickedDel.AddDynamic(PC, &ALROutGameController::OpenStageWidget);
	}
}

void ULRChapterWidget::OnOpenButtonClicked()
{
	OnChapterOpenClickedDel.Broadcast();
}
