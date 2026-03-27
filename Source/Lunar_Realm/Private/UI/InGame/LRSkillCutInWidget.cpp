// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRSkillCutInWidget.h"
#include "Units/Player/LRPlayerController.h"
#include "Components/Image.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/Texture2D.h"

void ULRSkillCutInWidget::InitCutIn(FName InCharacterID)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return;

	const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(InCharacterID);

	if (UTexture2D* LoadedImg = CharData.CutInImage.LoadSynchronous())
	{
		if (Img_Character)
		{
			Img_Character->SetBrushFromTexture(LoadedImg);
		}
	}

	if (SkillCutIn)
	{
		PlayAnimation(SkillCutIn);
	}

	if (AnimWind)
	{
		PlayAnimation(AnimWind);
	}

}

void ULRSkillCutInWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* InAnimation)
{
	Super::OnAnimationFinished_Implementation(InAnimation);

	if (InAnimation == SkillCutIn)
	{
		if (ALRPlayerController* PC = Cast<ALRPlayerController>(GetOwningPlayer()))
		{
			PC->EndSkillCutIn(this);
		}
	}
}
