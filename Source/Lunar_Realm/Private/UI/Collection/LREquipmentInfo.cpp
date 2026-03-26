// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREquipmentInfo.h"


#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Units/OutGame/LROutGameController.h"
#include "UI/Collection/LRCharacterStatusWidget.h"
#include "UI/Collection/LREnhancePageWidget.h"

#include "UI/Core/LRButtonWidget.h"

#include "UI/Common/LREquipmentCard.h"
#include "UI/Collection/LRSkillInfoWidget.h"
#include "UI/Collection/LREquipStatus.h"

void ULREquipmentInfo::NativeConstruct()
{
	Super::NativeConstruct();

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULREquipmentInfo::SetEquipIDCall);
	}
}

void ULREquipmentInfo::RefreshUI()
{
	Super::RefreshUI();

	FName SkillID = NAME_None;
	if (USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		if (EquipID.IsNone())
		{
			EquipID = SaveGameSubsystem->GetLeaderCharacterID();
		}

		if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
		{
			const FEquipmentStaticData& EquipmentStaticData = GameDataSubsystem->GetEquipmentStaticData(EquipID);
			SkillID = EquipmentStaticData.SkillIDs.IsValidIndex(0) ? EquipmentStaticData.SkillIDs[0] : NAME_None;
		}
	}
	SkillInfo->SetSkillID(SkillID);
	EquipmentCard->SetEquipID(EquipID);
	EquipmentStatus->SetEquipID(EquipID);
}

void ULREquipmentInfo::BindSubWidgets()
{
	Super::BindSubWidgets();
	if (Btn_Enhance)
	{
		Btn_Enhance->OnLRButtonClickedDel.RemoveAll(this);
		Btn_Enhance->OnLRButtonClickedDel.AddDynamic(this, &ULREquipmentInfo::OnEnhanceButtonClicked);
	}
}

void ULREquipmentInfo::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(EquipmentCard);
	SubWidgets.Add(SkillInfo);
	SubWidgets.Add(EquipmentStatus);
}

void ULREquipmentInfo::SetEquipIDCall(const FSelectedInfo& InInfo)
{
	if (InInfo.Type != ECollectionType::EQUIPMENT)
	{
		return;
	}

	SetEquipID(InInfo.ID);
}

void ULREquipmentInfo::SetEquipID(const FName& InID)
{
	EquipID = InID;
	RefreshUI();
}

void ULREquipmentInfo::OnEnhanceButtonClicked()
{
	UUIManagerSubsystem* UIManagerSubsystem = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (UIManagerSubsystem)
	{
		ULRBaseWidget* Widget = UIManagerSubsystem->OpenUIByID(EUIID::ENHANCE);
		if (!Widget)
		{
			return;
		}

		if (ULREnhancePageWidget* EnhanceWidget = Cast<ULREnhancePageWidget>(Widget))
		{
			FSelectedInfo SelectedInfo(ECollectionType::EQUIPMENT, EquipID);
			EnhanceWidget->SetIDByType(SelectedInfo);
		}
	}
}
