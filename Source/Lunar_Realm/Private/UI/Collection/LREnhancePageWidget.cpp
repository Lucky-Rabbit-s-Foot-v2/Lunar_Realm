// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhancePageWidget.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterEnhanceWidget.h"

void ULREnhancePageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterEnhance);
	SubWidgets.Add(Collection);
}

void ULREnhancePageWidget::SetCharacterID(FName InID)
{
	CharacterID = InID;
	RefreshUI();
}
