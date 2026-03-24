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

	MaxExpAmount = GetRequiredExpForLevel(CurrentLevel);

	FillSpeed = InGainedExp / 1.5f;
	if (FillSpeed < 20.0f) FillSpeed = 20.0f;

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
			}
			if (UTexture2D* LoadedGrade = CharData.GradeImage.LoadSynchronous())
			{
				if (GradeImage)
				{
					GradeImage->SetBrushFromTexture(LoadedGrade);
				}
			}
			if (NameText)
			{
				NameText->SetText(FText::FromString(CharData.CharacterName));
			}
		}
	}

	RefreshUI();
	bIsFilling = true;

	LR_INFO(TEXT("[%s] 시작Lv: %d | 시작Exp: %.1f | 얻은Exp: %.1f | 필요MaxExp: %.1f"), *CharacterID.ToString(), CurrentLevel, CurrentVisualExp, InGainedExp, MaxExpAmount);

}



void ULRExpSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	if (!bIsFilling) return;

	CurrentVisualExp = FMath::FInterpConstantTo(CurrentVisualExp, TargetExp, InDeltaTime, FillSpeed);

	if (CurrentVisualExp >= MaxExpAmount)
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
		ExpBar->SetPercent(CurrentVisualExp / MaxExpAmount);
	}
	else
	{
		LR_WARN(TEXT("ExpBar 위젯이 바인딩되지 않았거나 MaxExp가 0"));
	}
	if (CurrentExp)
	{
		int32 CurrentExpInt = FMath::RoundToInt(CurrentVisualExp);
		CurrentExp->SetText(FText::AsNumber(CurrentExpInt));
	}

	if (MaxExp)
	{
		int32 MaxExpInt = FMath::RoundToInt(MaxExpAmount);
		MaxExp->SetText(FText::AsNumber(MaxExpInt));
	}
}

void ULRExpSlotWidget::HandleLevelUp()
{
	CurrentVisualExp -= MaxExpAmount;
	TargetExp -= MaxExpAmount;
	CurrentLevel++;

	MaxExpAmount = GetRequiredExpForLevel(CurrentLevel);

	if (LevelUpAnim)
	{
		PlayAnimation(LevelUpAnim);
	}
}

float ULRExpSlotWidget::GetRequiredExpForLevel(int32 InLevel) const
{
	float ReqExp = 500.0f;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>())
		{
			float CurveExp = DataSys->GetBaseStatAtLevel(ELRStatusType::EXP, InLevel);
			if (CurveExp > 0.0f)
			{
				ReqExp = CurveExp;
			}
			else
			{
				LR_WARN(TEXT("레벨 %d의 경험치 커브 데이터를 못 찾음"), InLevel);
			}
		}
	}
	return ReqExp;
}
