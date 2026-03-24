// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipStatus.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "Engine/GameInstance.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"


void ULREquipStatus::SetEquipID(const FName& InID)
{
	ID = InID;
	RefreshUI();
}

void ULREquipStatus::RefreshUI()
{
	Super::RefreshUI();

	UpdateEquipData();
	UpdateEquipStatus();
}

void ULREquipStatus::UpdateEquipData()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

	if (!GameDataSubsystem || !CollectionSubsystem)
	{
		return;
	}

	if (ID.IsNone())
	{
		return;
	}

	EquipmentStaticData = GameDataSubsystem->GetEquipmentStaticData(ID);
	TArray<FEquipmentInstance> Instances = CollectionSubsystem->GetEquipmentInstancesByKey(ID);
	if(Instances.Num() > 0)
	{
		EquipmentInstance = Instances[0];
	}
	else
	{
		EquipmentInstance = FEquipmentInstance();
	}
}

void ULREquipStatus::UpdateEquipStatus()
{
	if (ID.IsNone())
	{
		return;
	}

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	UpdateLevel();
	UpdateEXP();
	UpdateSetType();
	UpdateType();
	UpdateATK();
	UpdateDEF();
}

void ULREquipStatus::UpdateLevel()
{
	int32 CurrentLevel = EquipmentInstance.CurrentLevel;
	Level->SetText(FText::AsNumber(CurrentLevel));
}

void ULREquipStatus::UpdateEXP()
{
	UGameDataSubsystem* GameDataSys = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	int32 ExpToNextLevel = static_cast<int32>(GameDataSys->GetBaseStatAtLevel(ELRStatusType::EXP, EquipmentInstance.CurrentLevel));

	int32 CurrentExp = EquipmentInstance.CurrentExp;

	float ExpProgress = (ExpToNextLevel > 0) ? static_cast<float>(CurrentExp) / ExpToNextLevel : 0.0f;
	Bar->SetPercent(ExpProgress);

	FString ExpText = FString::Printf(TEXT("%d / %d (%.1f%%)"), CurrentExp, ExpToNextLevel, ExpProgress * 100.0f);
	EXP->SetText(FText::FromString(ExpText));
}

void ULREquipStatus::UpdateSetType()
{
	FString SetTypeName = UEnum::GetValueAsString(EquipmentStaticData.SetType);
	SetTypeName.RemoveAt(0, SetTypeName.Find(TEXT("::")) + 2);
	SetType->SetText(FText::FromString(SetTypeName));
}

void ULREquipStatus::UpdateType()
{
	FString TypeName = UEnum::GetValueAsString(EquipmentStaticData.ItemType);
	TypeName.RemoveAt(0, TypeName.Find(TEXT("::")) + 2);
	Type->SetText(FText::FromString(TypeName));
}

void ULREquipStatus::UpdateATK()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	float EquipmentATKBonus = GameDataSubsystem->GetCharacterFinalStat(ID, ELRStatusType::ATK, EquipmentInstance.CurrentLevel);
	ATK->SetText(FText::AsNumber(EquipmentATKBonus));
}

void ULREquipStatus::UpdateDEF()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	float EquipmentDEFBonus = GameDataSubsystem->GetCharacterFinalStat(ID, ELRStatusType::DEF, EquipmentInstance.CurrentLevel);
	DEF->SetText(FText::AsNumber(EquipmentDEFBonus));
}