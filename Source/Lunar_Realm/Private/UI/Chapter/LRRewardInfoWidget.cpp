// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRRewardInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void ULRRewardInfoWidget::RefreshUI()
{
	Super::RefreshUI();

	Txt_Amount->SetText(FText::AsNumber(RewardAmount));
}

void ULRRewardInfoWidget::SetRewardAmount(const int32 InAmount)
{
	RewardAmount = InAmount;

	RefreshUI();
}
