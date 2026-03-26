// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterPageWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Engine/AssetManager.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Chapter/LRChapterWidget.h"
#include "UI/Chapter/LRStagePageWidget.h"
#include "TimerManager.h"
#include "Animation/WidgetAnimation.h"

void ULRChapterPageWidget::InitializeUI()
{
	Super::InitializeUI();
	Chapter1->SetChapterID(FName("LAKE"));
	Chapter2->SetChapterID(FName("OCEAN"));
	Chapter3->SetChapterID(FName("DESERT"));
}

void ULRChapterPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	
	ChapterWidgets.Add(Chapter1);
	ChapterWidgets.Add(Chapter2);
	ChapterWidgets.Add(Chapter3);
}

void ULRChapterPageWidget::PlayFadeOutAndOpenStage(FName InChapterID)
{
	if (bIsTransitioning) return;
	bIsTransitioning = true;

	if (!Anim_FadeOut)
	{
		LoadBackgroundAndOpenStage(InChapterID);
		return;
	}

	PlayAnimation(Anim_FadeOut);

	GetWorld()->GetTimerManager().SetTimer(
		TransitionTimer,
		[this, InChapterID]()
		{
			bIsReturningFromStage = true;
			LoadBackgroundAndOpenStage(InChapterID);
		},
		Anim_FadeOut->GetEndTime(),
		false
	);
}

void ULRChapterPageWidget::PlayFadeInAnimation()
{
	if (Anim_FadeIn)
	{
		PlayAnimation(Anim_FadeIn);
	}
}

void ULRChapterPageWidget::OpenUI()
{
	Super::OpenUI();

	bIsTransitioning = false;

	if (bIsReturningFromStage)
	{
		if (Anim_FadeIn)
		{
			PlayAnimation(Anim_FadeIn);
		}
		else if (Anim_FadeOut)
		{
			PlayAnimation(Anim_FadeOut, 0.0f, 1, EUMGSequencePlayMode::Reverse);
		}

		bIsReturningFromStage = false;
	}
}

void ULRChapterPageWidget::LoadBackgroundAndOpenStage(FName InChapterID)
{
	UGameDataSubsystem* DataSys = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return; 

	const FChapterStaticData& ChapterData = DataSys->GetChapterStaticData(InChapterID);
	FSoftObjectPath BgPath = ChapterData.ChapterBackground.ToSoftObjectPath();

	UAssetManager::GetStreamableManager().RequestAsyncLoad(BgPath, FStreamableDelegate::CreateLambda([this, InChapterID]()
		{
			OpenStageWidget(InChapterID);
		}));
}

void ULRChapterPageWidget::OpenStageWidget(FName InChapterID)
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManager) return; 

	ULRStagePageWidget* StagePage = Cast<ULRStagePageWidget>(UIManager->OpenUIByID(EUIID::STAGE));
	if (!StagePage) return; 

	StagePage->SetChapterID(InChapterID);
}
