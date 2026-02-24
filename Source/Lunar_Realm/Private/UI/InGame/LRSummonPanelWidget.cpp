// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRSummonPanelWidget.h"

#include "UI/InGame/LRSummonSlotWidget.h"
#include "Units/Player/Component/LRSummonComponent.h"
#include "GameFramework/Pawn.h"

void ULRSummonPanelWidget::InitializeUI()
{
	Super::InitializeUI();
	SlotArray.Empty();
	if (SummonSlot_0) SlotArray.Add(SummonSlot_0);
	if (SummonSlot_1) SlotArray.Add(SummonSlot_1);
	if (SummonSlot_2) SlotArray.Add(SummonSlot_2);
	if (SummonSlot_3) SlotArray.Add(SummonSlot_3);
}

void ULRSummonPanelWidget::RefreshUI()
{
	Super::RefreshUI();

	InitPanel();
}

void ULRSummonPanelWidget::InitPanel()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn) 
	{
		return;
	}
	ULRSummonComponent* SummonComp = OwningPawn->GetComponentByClass<ULRSummonComponent>();
	if (!SummonComp)
	{
		return;
	}
	const TArray<FName>& MyDeck = SummonComp->GetSummonDeck();

	for (int32 i = 0; i < SlotArray.Num(); i++)
	{
		if (SlotArray[i])
		{
			if (MyDeck.IsValidIndex(i))
			{
				SlotArray[i]->InitSlot(i, MyDeck[i]);
				SlotArray[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				SlotArray[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void ULRSummonPanelWidget::OpenUI()
{
	Super::OpenUI();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
