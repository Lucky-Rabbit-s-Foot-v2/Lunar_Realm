// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterEnhanceWidget.h"

#include "Components/Image.h"

#include "UI/Common/LRCharacterCard.h"
#include "UI/Collection/LRCharacterStatusWidget.h"
#include "UI/Collection/LREnhanceButtonWidget.h"

void ULRCharacterEnhanceWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterCard);
	SubWidgets.Add(CharacterStatus);
}

void ULRCharacterEnhanceWidget::RefreshUI()
{
	Super::RefreshUI();
}

void ULRCharacterEnhanceWidget::SetCharacterID(const FName& InID)
{
	ID = InID;

	CharacterCard->SetCharacterID(ID);
	CharacterStatus->SetCharacterID(ID);
	RefreshUI();
}
