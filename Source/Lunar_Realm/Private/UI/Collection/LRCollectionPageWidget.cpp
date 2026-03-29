// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollectionPageWidget.h"

#include "Components/WidgetSwitcher.h"

#include "Units/OutGame/LROutGameController.h"

#include "UI/Collection/LRCollection.h"
#include "UI/Collection/LRCharacterInfoWidget.h"
#include "UI/Collection/LREquipmentInfo.h"

#include "Engine/GameInstance.h"
#include "Subsystems/SaveGameSubsystem.h"

void ULRCollectionPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(EquipmentInfo);
	SubWidgets.Add(CharacterInfo);
	SubWidgets.Add(Collection);
}

void ULRCollectionPageWidget::OpenUI()
{
	Super::OpenUI();

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		ID = PC->GetSelectedID();
		Type = PC->GetSelectedType();
	}
	
	if (ID.IsNone())
	{
		if (USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
		{

			ID = SaveGameSubsystem->GetLeaderCharacterID();
			Type = ECollectionType::CHARACTER;
		}
	}

	SetIDAndType(ID, Type);
}

void ULRCollectionPageWidget::SetIDAndType(FName InID, ECollectionType InType)
{
	ID = InID;
	Type = InType;

	SwitchWidgetByType(Type);
	
	switch (Type)
	{
	case ECollectionType::CHARACTER:
		CharacterInfo->SetCharacterID(ID);
		break;

	case ECollectionType::EQUIPMENT:
		EquipmentInfo->SetEquipID(ID);
		break;

	default:
		break;
	}
}

void ULRCollectionPageWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALROutGameController* PC = Cast<ALROutGameController>(Controller);

	if (PC)
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULRCollectionPageWidget::SetIDAndType);
	}
}

void ULRCollectionPageWidget::SwitchWidgetByType(ECollectionType InType)
{
	Switcher->SetActiveWidgetIndex(static_cast<int32>(InType));
}
