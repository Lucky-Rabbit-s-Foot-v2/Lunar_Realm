// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

#include "Units/OutGame/LROutGameController.h"


void ULRSlotWidget::BindProperties()
{
	Super::BindProperties();

	Button->OnClicked.AddUniqueDynamic(this, &ULRSlotWidget::OnSlotClicked);
}

void ULRSlotWidget::UnbindProperties()
{
	Button->OnClicked.Clear();
	OnSlotClickedDel.Clear();
	
	Super::UnbindProperties();
}

void ULRSlotWidget::RefreshUI()
{
	Super::RefreshUI();

	SetGradeImage();
	SetIconImage();
	SetSelected(bIsSelected);
}

void ULRSlotWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);
	if (PC)
	{
		OnSlotClickedDel.AddUniqueDynamic(PC, &ALROutGameController::OnSelectedSlotWidget);
		OnSlotToggledDel.AddUniqueDynamic(PC, &ALROutGameController::OnSelectedSlotToggled);
	}
}

void ULRSlotWidget::SetSlotIndex(int32 InIndex)
{
	SlotIndex = InIndex;
}

void ULRSlotWidget::SetID(FName InID)
{
	ID = InID;

	RefreshUI();
}

void ULRSlotWidget::OnSlotClicked()
{
	SetSelected(true);
	OnSlotClickedDel.Broadcast(this);
}

void ULRSlotWidget::SetGradeImage()
{
	if (ID.IsNone())
	{
		Img_Grade->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
}

void ULRSlotWidget::SetIconImage()
{
	if (ID.IsNone())
	{
		Image->SetBrushFromTexture(EmptySlotTexture);
		return;
	}
}

void ULRSlotWidget::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;

	if (bIsSelected)
	{
		Img_Selected->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Img_Selected->SetVisibility(ESlateVisibility::Hidden);
	}

	OnSlotToggledDel.Broadcast(bIsSelected);
}