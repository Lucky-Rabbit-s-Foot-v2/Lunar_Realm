// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhanceButtonWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "Engine/GameInstance.h"
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

void ULREnhanceButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonText = FText::Format(NSLOCTEXT("LREnhanceButtonWidget", "Text", "+{0} 강화"), FText::AsNumber(EnhanceCount));

	if (Text)
	{
		Text->SetText(ButtonText);
	}
}

void ULREnhanceButtonWidget::RefreshUI()
{
	Super::RefreshUI();

	UCollectionSubsystem* CollectionSub = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	if (CurrentTypeIndex == 0)
	{
		FCharacterInstance CharacterData = CollectionSub->GetCharacterInstance(ID);
		CalculateExp(CharacterData.CurrentLevel, CharacterData.CurrentExp, MaxLevel);
	}
	else if(CurrentTypeIndex == 1)
	{
		TArray<FEquipmentInstance> EquipmentInstances = CollectionSub->GetEquipmentInstancesByKey(ID);
		if (EquipmentInstances.Num() == 0)
		{
			EnhanceExp = 0;
			EnhanceCost = 0;
			Txt_Cost->SetText(FText::AsNumber(EnhanceCost));
			return;
		}

		FEquipmentInstance EquipmentData = EquipmentInstances[0];
		CalculateExp(EquipmentData.CurrentLevel, EquipmentData.CurrentExp, MaxLevel);
	}
	Txt_Cost->SetText(FText::AsNumber(EnhanceCost));
}

void ULREnhanceButtonWidget::OnButtonClicked()
{
	UCollectionSubsystem* CollectionSub = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	UCurrencySubsystem* CurrencySub = GetGameInstance()->GetSubsystem<UCurrencySubsystem>();

	if (CurrencySub->SpendCurrency(ELRCurrencyType::Gold, EnhanceCost))
	{
		if (CurrentTypeIndex == 0)
		{
			CollectionSub->AddCharacterExp(ID, EnhanceExp);
		}
		else if (CurrentTypeIndex == 1)
		{
			FEquipmentInstance EquipmentInstance = CollectionSub->GetEquipmentInstancesByKey(ID)[0];
			CollectionSub->AddEquipmentExp(EquipmentInstance.InstanceID, EnhanceExp);
		}
	}

	OnLRButtonClickedDel.Broadcast();
}

void ULREnhanceButtonWidget::SetCharacterID(const FName& InID)
{
	ID = InID;
	CurrentTypeIndex = 0;
	RefreshUI();
}

void ULREnhanceButtonWidget::SetEquipID(const FName& InID)
{
	ID = InID;
	CurrentTypeIndex = 1;
	RefreshUI();
}

void ULREnhanceButtonWidget::CalculateExp(int32 CurrentLevel, int32 CurrentExp, int32 InMaxLevel)
{
	EnhanceExp = 0;
	UGameDataSubsystem* GameDataSys = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	for (int32 i = CurrentLevel; i < CurrentLevel + EnhanceCount && i < InMaxLevel; i++)
	{
		int32 RequiredExpFloat = static_cast<int32>(GameDataSys->GetBaseStatAtLevel(ELRStatusType::EXP, i));
		EnhanceExp += RequiredExpFloat;
	}
	EnhanceExp -= CurrentExp;
	EnhanceCost = EnhanceExp * CostPerExp;
}
