// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollection.h"

#include "Components/WidgetSwitcher.h"

#include "UI/Core/LRButtonWidget.h"

#include "UI/Collection/LRCharacterCollection.h"
#include "UI/Collection/LREquipCollection.h"

void ULRCollection::InitializeUI()
{
	Super::InitializeUI();

	OnBtnCharacterClicked();
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

		int32 Index = 0;
		SetButtonEnable(Index);
		Switcher_Collection->SetActiveWidgetIndex(Index);
	}
}

void ULRCollection::OnBtnEquipClicked()
{
	if (Switcher_Collection)
	{
		EquipCollection->RefreshUI();

		int32 Index = 1;
		SetButtonEnable(Index);
		Switcher_Collection->SetActiveWidgetIndex(Index);
	}
}

void ULRCollection::SetButtonEnable(int32 Index)
{
	if (Index == 0)
	{
		Btn_Character->SetIsEnabled(false);
		Btn_Character->SetColorAndOpacity(FLinearColor::Gray);

		Btn_Equip->SetIsEnabled(true);
		Btn_Equip->SetColorAndOpacity(FLinearColor::White);
	}
	else
	{
		Btn_Character->SetIsEnabled(true);
		Btn_Character->SetColorAndOpacity(FLinearColor::White);

		Btn_Equip->SetIsEnabled(false);
		Btn_Equip->SetColorAndOpacity(FLinearColor::Gray);
	}
}
