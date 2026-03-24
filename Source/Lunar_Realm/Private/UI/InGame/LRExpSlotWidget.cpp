// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRExpSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"


void ULRExpSlotWidget::SetSlotInfo(FName InCharID, int32 InOldLevel, float InOldExp, float InGainedExp)
{
	CharacterID = InCharID;
	CurrentLevel = InOldLevel;
	CurrentVisualExp = static_cast<float>(InOldExp);
	TargetExp = static_cast<float>(InOldExp + InGainedExp);

	MaxExp = GetRequiredExpForLevel(CurrentLevel);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>())
		{
			const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(CharacterID);
			if (UTexture2D* LoadedIcon = CharData.CharacterTexture.LoadSynchronous())
			{
				if (CharIcon)
				{
					CharIcon->SetBrushFromTexture(LoadedIcon);
				}
				else
				{
					LR_WARN(TEXT("CharIcon 위젯이 바인딩되지 않음! 블루프린트 확인 필요."));
				}
			}
		}
	}

	RefreshUI();
	bIsFilling = true;
}



void ULRExpSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsFilling) return;

	CurrentVisualExp = FMath::FInterpConstantTo(CurrentVisualExp, TargetExp, InDeltaTime, FillSpeed);

	if (CurrentVisualExp >= MaxExp)
	{
		HandleLevelUp();
	}

	RefreshUI();

	if (CurrentVisualExp >= TargetExp)
	{
		CurrentVisualExp = TargetExp;
		bIsFilling = false;
	}
}

void ULRExpSlotWidget::RefreshUI()
{
	Super::RefreshUI();

	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(CurrentLevel));
	}
	else
	{
		LR_WARN(TEXT("LevelText 위젯이 바인딩되지 않음"));
	}
	if (ExpBar)
	{
		ExpBar->SetPercent(CurrentVisualExp / MaxExp);
	}
	else
	{
		LR_WARN(TEXT("ExpBar 위젯이 바인딩되지 않았거나 MaxExp가 0"));
	}
}

void ULRExpSlotWidget::HandleLevelUp()
{
	CurrentVisualExp -= MaxExp;
	TargetExp -= MaxExp;
	CurrentLevel++;

	MaxExp = GetRequiredExpForLevel(CurrentLevel);

	if (LevelUpAnim)
	{
		PlayAnimation(LevelUpAnim);
	}
}

float ULRExpSlotWidget::GetRequiredExpForLevel(int32 InLevel) const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>())
		{
			return DataSys->GetBaseStatAtLevel(ELRStatusType::EXP, InLevel);
		}
	}
	return 500.0f;
}
