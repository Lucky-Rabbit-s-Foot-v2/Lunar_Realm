// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollection.h"

#include "Components/WidgetSwitcher.h"

#include "UI/Core/LRButtonWidget.h"

#include "UI/Collection/LRCharacterCollection.h"
#include "UI/Collection/LREquipCollection.h"

void ULRCollection::InitializeUI()
{
	Super::InitializeUI();

	if (Switcher_Collection) Switcher_Collection->SetActiveWidgetIndex(0);
}

void ULRCollection::BindProperties()
{
	Super::BindProperties();

	Btn_Character->OnLRButtonClickedDel.RemoveAll(this);
	Btn_Character->OnLRButtonClickedDel.AddDynamic(this, &ULRCollection::OnBtnCharacterClicked);
	
	Btn_Equip->OnLRButtonClickedDel.RemoveAll(this);
	Btn_Equip->OnLRButtonClickedDel.AddDynamic(this, &ULRCollection::OnBtnEquipClicked);
}

void ULRCollection::UnbindProperties()
{
	Btn_Character->OnLRButtonClickedDel.RemoveAll(this);
	Btn_Equip->OnLRButtonClickedDel.RemoveAll(this);

	Super::UnbindProperties();
}

void ULRCollection::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterCollection);
	SubWidgets.Add(EquipCollection);
}

void ULRCollection::OnBtnCharacterClicked()
{
	if (Switcher_Collection)
	{
		CharacterCollection->RefreshUI();
		Switcher_Collection->SetActiveWidgetIndex(0);
	}
}

void ULRCollection::OnBtnEquipClicked()
{
	if (Switcher_Collection)
	{
		EquipCollection->RefreshUI();
		Switcher_Collection->SetActiveWidgetIndex(1);
	}
}
