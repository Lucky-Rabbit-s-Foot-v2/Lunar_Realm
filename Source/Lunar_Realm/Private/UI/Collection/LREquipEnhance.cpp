// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipEnhance.h"


#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Common/LREquipmentCard.h"
#include "UI/Collection/LREquipStatus.h"
#include "UI/Collection/LREnhanceButtonWidget.h"

void ULREquipEnhance::BindProperties()
{
	Super::BindProperties();

	Enhance1->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULREquipEnhance::RefreshUICall);
	Enhance5->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULREquipEnhance::RefreshUICall);
	Enhance10->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULREquipEnhance::RefreshUICall);
}

void ULREquipEnhance::UnbindProperties()
{
	Enhance1->OnLRButtonClickedDel.RemoveDynamic(this, &ULREquipEnhance::RefreshUICall);
	Enhance5->OnLRButtonClickedDel.RemoveDynamic(this, &ULREquipEnhance::RefreshUICall);
	Enhance10->OnLRButtonClickedDel.RemoveDynamic(this, &ULREquipEnhance::RefreshUICall);

	Super::UnbindProperties();
}

void ULREquipEnhance::RefreshUICall()
{
	RefreshUI();
}

void ULREquipEnhance::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(EquipCard);
	SubWidgets.Add(EquipStatus);
	SubWidgets.Add(Enhance1);
	SubWidgets.Add(Enhance5);
	SubWidgets.Add(Enhance10);
}

void ULREquipEnhance::SetEquipID(const FName& InID)
{
	ID = InID;

	EquipCard->SetEquipID(ID);
	EquipStatus->SetEquipID(ID);
	Enhance1->SetEquipID(ID);
	Enhance5->SetEquipID(ID);
	Enhance10->SetEquipID(ID);
	RefreshUI();
}
