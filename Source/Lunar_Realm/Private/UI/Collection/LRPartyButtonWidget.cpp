// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartyButtonWidget.h"

#include "Units/OutGame/LROutGameController.h"

void ULRPartyButtonWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);
	if (PC)
	{
		OnPartyButtonClickedDel.AddUniqueDynamic(PC, &ALROutGameController::RequestUpdateSelectedInfo);
	}
}

void ULRPartyButtonWidget::OnButtonClicked()
{
	FSelectedInfo Info(ECollectionType::NONE, NAME_None, -1, Type);
	OnPartyButtonClickedDel.Broadcast(Info);
}
