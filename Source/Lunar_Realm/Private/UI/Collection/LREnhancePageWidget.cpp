// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhancePageWidget.h"

#include "Components/WidgetSwitcher.h"

#include "Engine/GameInstance.h"

#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterEnhanceWidget.h"
#include "UI/Collection/LREquipEnhance.h"

#include "Units/OutGame/LROutGameController.h"

void ULREnhancePageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterEnhance);
	SubWidgets.Add(EquipEnhance);
	SubWidgets.Add(Collection);
}

void ULREnhancePageWidget::OpenUI()
{
	Super::OpenUI();

	if(ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		ID = PC->GetSelectedID();
		Type = PC->GetSelectedType();
	}

	if (ID.IsNone())
	{
		USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
		if (SaveGameSubsystem)
		{
			ID = SaveGameSubsystem->GetLeaderCharacterID();
			Type = ECollectionType::CHARACTER;
		}
	}

	SetIDAndType(ID, Type);
}

void ULREnhancePageWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);
	if (PC)
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULREnhancePageWidget::SetIDAndType);
	}
}

void ULREnhancePageWidget::SetIDAndType(FName InID, ECollectionType InType)
{
	ID = InID;
	Type = InType;

	SwitchWidgetByType(Type);

	switch (Type)
	{
	case ECollectionType::CHARACTER:
		CharacterEnhance->SetCharacterID(ID);
		break;

	case ECollectionType::EQUIPMENT:
		EquipEnhance->SetEquipID(ID);
		break;
	
	default:
		break;
	}
}

void ULREnhancePageWidget::SwitchWidgetByType(ECollectionType InType)
{
	Switcher->SetActiveWidgetIndex(static_cast<int32>(InType));
}
