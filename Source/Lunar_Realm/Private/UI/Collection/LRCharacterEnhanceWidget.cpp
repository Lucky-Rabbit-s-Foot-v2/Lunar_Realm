// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterEnhanceWidget.h"

#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Common/LRCharacterCard.h"
#include "UI/Collection/LRCharacterStatusWidget.h"
#include "UI/Collection/LREnhanceButtonWidget.h"

void ULRCharacterEnhanceWidget::BindProperties()
{
	Super::BindProperties();

	Enhance1->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRCharacterEnhanceWidget::RefreshUICall);
	Enhance5->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRCharacterEnhanceWidget::RefreshUICall);
	Enhance10->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRCharacterEnhanceWidget::RefreshUICall);
}

void ULRCharacterEnhanceWidget::UnbindProperties()
{
	Enhance1->OnLRButtonClickedDel.RemoveDynamic(this, &ULRCharacterEnhanceWidget::RefreshUICall);
	Enhance5->OnLRButtonClickedDel.RemoveDynamic(this, &ULRCharacterEnhanceWidget::RefreshUICall);
	Enhance10->OnLRButtonClickedDel.RemoveDynamic(this, &ULRCharacterEnhanceWidget::RefreshUICall);

	Super::UnbindProperties();
}

void ULRCharacterEnhanceWidget::RefreshUICall()
{
	RefreshUI();
}

void ULRCharacterEnhanceWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterCard);
	SubWidgets.Add(CharacterStatus);
	SubWidgets.Add(Enhance1);
	SubWidgets.Add(Enhance5);
	SubWidgets.Add(Enhance10);
}

void ULRCharacterEnhanceWidget::SetCharacterID(const FName& InID)
{
	ID = InID;

	if (ID.IsNone())
	{
		Enhance1->SetIsEnabled(false);
		Enhance5->SetIsEnabled(false);
		Enhance10->SetIsEnabled(false);
	}
	else
	{
		Enhance1->SetIsEnabled(true);
		Enhance5->SetIsEnabled(true);
		Enhance10->SetIsEnabled(true);
	}

	CharacterCard->SetCharacterID(ID);
	CharacterStatus->SetCharacterID(ID);
	Enhance1->SetCharacterID(ID);
	Enhance5->SetCharacterID(ID);
	Enhance10->SetCharacterID(ID);
	RefreshUI();
}
