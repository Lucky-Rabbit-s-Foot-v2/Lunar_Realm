// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollectionPageWidget.h"

#include "UI/Collection/LRCollection.h"

void ULRCollectionPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterInfo);
	SubWidgets.Add(Collection);
}

