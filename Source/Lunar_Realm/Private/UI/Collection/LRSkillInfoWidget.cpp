// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRSkillInfoWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

#include "Subsystems/GameDataSubsystem.h"


void ULRSkillInfoWidget::RefreshUI()
{
	Super::RefreshUI();
	
	if (UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>())
	{
		UTexture2D* SkillIcon = GameDataSubsystem->GetSkillIcon(ID).LoadSynchronous();
		Icon->SetBrushFromTexture(SkillIcon ? SkillIcon : DefaultIcon);

		//FText SkillDescription = GameDataSubsystem->GetSkillDescription(ID);
		//Description->SetText(!SkillDescription.IsEmpty() ? SkillDescription : DefaultDescription);
	}

}

void ULRSkillInfoWidget::SetSkillID(const FName& InID)
{
	ID = InID;
	RefreshUI();
}