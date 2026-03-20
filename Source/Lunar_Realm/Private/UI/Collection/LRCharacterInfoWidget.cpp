// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterInfoWidget.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

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
		if(CharacterID.IsNone())
		{
			CharacterID = SaveGameSubsystem->GetLeaderCharacterID();
		}

		FCharacterStaticData CharacterStaticData = GetGameInstance()->GetSubsystem<UGameDataSubsystem>()->GetCharacterStaticData(CharacterID);
		Img_Main->SetBrushFromTexture(CharacterStaticData.PortraitIcon.LoadSynchronous());
		Img_Grade->SetBrushFromTexture(CharacterStaticData.GradeImage.LoadSynchronous());

		FName SkillID = CharacterStaticData.SkillIDs.IsValidIndex(0) ? CharacterStaticData.SkillIDs[0] : NAME_None;
		if (SkillID != NAME_None)
		{
			if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
			{
				UTexture2D* SkillIcon = GameDataSubsystem->GetSkillIcon(SkillID).LoadSynchronous();
				Img_Skill->SetBrushFromTexture(SkillIcon ? SkillIcon : DefaultImage);
			}
		}
		else
		{
			Img_Skill->SetBrushFromTexture(DefaultImage);
		}
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
