// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollection.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void ULRCollection::NativeConstruct()
{
	Super::NativeConstruct();


	if (Switcher_Collection) Switcher_Collection->SetActiveWidgetIndex(0);
}

void ULRCollection::BindProperties()
{
	Super::BindProperties();

	if (Btn_Character) Btn_Character->OnClicked.AddDynamic(this, &ULRCollection::OnBtnCharacterClicked);
	if (Btn_Equip) Btn_Equip->OnClicked.AddDynamic(this, &ULRCollection::OnBtnEquipClicked);
}

void ULRCollection::UnbindProperties()
{
	if (Btn_Character) Btn_Character->OnClicked.Clear();
	if (Btn_Equip) Btn_Equip->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRCollection::OnBtnCharacterClicked()
{
	if(Switcher_Collection)
	{
		Switcher_Collection->SetActiveWidgetIndex(0);
	}
}

void ULRCollection::OnBtnEquipClicked()
{
	if (Switcher_Collection)
	{
		Switcher_Collection->SetActiveWidgetIndex(1);
	}
}
