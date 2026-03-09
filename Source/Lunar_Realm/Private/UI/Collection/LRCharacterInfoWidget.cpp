// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterInfoWidget.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Components/Image.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Units/OutGame/LROutGameController.h"
#include "UI/Collection/LRCharacterStatusWidget.h"


void ULRCharacterInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->OnSelectedCharacterChangedDel.AddUniqueDynamic(this, &ULRCharacterInfoWidget::SetCharacterID);
	}
}

void ULRCharacterInfoWidget::RefreshUI()
{
	Super::RefreshUI();

	if (USaveGameSubsystem* SaveGameSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
	{
		FCharacterStaticData CharacterStaticData = GetGameInstance()->GetSubsystem<UGameDataSubsystem>()->GetCharacterStaticData(CharacterID);
		Img_Main->SetBrushFromTexture(CharacterStaticData.PortraitIcon.LoadSynchronous());
	}

	CharacterStatus->SetCharacterID(CharacterID);
	CharacterStatus->RefreshUI();
}

void ULRCharacterInfoWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(CharacterStatus);
}

void ULRCharacterInfoWidget::SetCharacterID(FName InID)
{
	CharacterID = InID;
	RefreshUI();
}
