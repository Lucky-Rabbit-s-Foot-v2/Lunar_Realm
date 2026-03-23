// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterInfoWidget.h"

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

#include "UI/Common/LRCharacterCard.h"
#include "UI/Collection/LRSkillInfoWidget.h"

void ULRCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->OnSelectedCharacterChangedDel.AddUniqueDynamic(this, &ULRCharacterInfoWidget::SetCharacterIDCall);
	}
}

void ULRCharacterInfoWidget::RefreshUI()
{
	Super::RefreshUI();

	FName SkillID = NAME_None;
	if (USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		if (CharacterID.IsNone())
		{
			CharacterID = SaveGameSubsystem->GetLeaderCharacterID();
		}
		
		if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
		{
			const FCharacterStaticData& CharacterStaticData = GameDataSubsystem->GetCharacterStaticData(CharacterID);
			SkillID = CharacterStaticData.SkillIDs.IsValidIndex(0) ? CharacterStaticData.SkillIDs[0] : NAME_None;
		}
	}
	SkillInfo->SetSkillID(SkillID);
	CharacterCard->SetCharacterID(CharacterID);
	CharacterStatus->SetCharacterID(CharacterID);
}

void ULRCharacterInfoWidget::BindSubWidgets()
{
	Super::BindSubWidgets();
	if (Btn_Enhance)
	{
		Btn_Enhance->OnLRButtonClickedDel.RemoveAll(this);
		Btn_Enhance->OnLRButtonClickedDel.AddDynamic(this, &ULRCharacterInfoWidget::OnEnhanceButtonClicked);
	}
}

void ULRCharacterInfoWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterCard);
	SubWidgets.Add(SkillInfo);
	SubWidgets.Add(CharacterStatus);
}

void ULRCharacterInfoWidget::SetCharacterIDCall(FName InID)
{
	SetCharacterID(InID);
}

void ULRCharacterInfoWidget::SetCharacterID(const FName& InID)
{
	CharacterID = InID;
	RefreshUI();
}

void ULRCharacterInfoWidget::OnEnhanceButtonClicked()
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
			EnhanceWidget->SetCharacterID(CharacterID);
		}
	}
}
