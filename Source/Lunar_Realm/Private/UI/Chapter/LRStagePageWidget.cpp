// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStagePageWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"

#include "UI/Chapter/LRStageWidget.h"

#include "Units/LRControllerBase.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"

#include "TimerManager.h"
#include "Animation/WidgetAnimation.h"

void ULRStagePageWidget::InitializeUI()
{
	Super::InitializeUI();

	if (ComboBox)
	{
		ComboBox->ClearOptions();
		ComboBox->AddOption(TEXT("LAKE"));
		ComboBox->AddOption(TEXT("OCEAN"));
		ComboBox->AddOption(TEXT("DESERT"));

		ComboBox->SetSelectedOption(TEXT("LAKE"));

		ComboBox->OnSelectionChanged.RemoveAll(this);
		ComboBox->OnSelectionChanged.AddDynamic(this, &ULRStagePageWidget::OnChapterSelectionChanged);
	}	
}

void ULRStagePageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Stage1);
	SubWidgets.Add(Stage2);
	SubWidgets.Add(Stage3);
	SubWidgets.Add(Stage4);
	SubWidgets.Add(Stage5);
}

void ULRStagePageWidget::OpenUI()
{
	Super::OpenUI();

	PlayAnimation(Anim_FadeIn);
}

void ULRStagePageWidget::SetChapterID(FName InID)
{
	CurrentChapterID = InID;
	if (ComboBox)
	{
		ComboBox->SetSelectedOption(CurrentChapterID.ToString());
	}

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const FChapterStaticData& ChapterData = GameDataSubsystem->GetChapterStaticData(CurrentChapterID);
	Img_BG->SetBrushFromTexture(ChapterData.ChapterBackground.LoadSynchronous());

	TArray<FName> StageIDs = GameDataSubsystem->GetAllStageIDsByChapterID(CurrentChapterID);
	SetStageData(StageIDs);

	RefreshUI();
}

void ULRStagePageWidget::SetStageData(const TArray<FName>& StageIDs)
{
	for (int32 i = 0; i < StageIDs.Num() && i < SubWidgets.Num(); ++i)
	{
		if (ULRStageWidget* StageWidget = Cast<ULRStageWidget>(SubWidgets[i]))
		{
			StageWidget->SetStageID(StageIDs[i]);
		}
	}

	UpdatePaths(StageIDs);
}

void ULRStagePageWidget::OnChapterSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Direct || SelectedItem == CurrentChapterID.ToString())
	{
		return;
	}

	if (Anim_FadeOut)
	{
		PlayAnimation(Anim_FadeOut);

		float FadeOutTime = Anim_FadeOut->GetEndTime(); + 0.15f;

		GetWorld()->GetTimerManager().SetTimer(
			ChapterChangeTimer,
			[this, SelectedItem]()
			{
				SetChapterID(FName(SelectedItem));

				if (Anim_FadeIn)
				{
					PlayAnimation(Anim_FadeIn);
				}
			},
			FadeOutTime,
			false
		);
	}
	else
	{
		SetChapterID(FName(SelectedItem));
	}
}

void ULRStagePageWidget::UpdatePaths(const TArray<FName>& InStageIDs)
{
	if (UStageManagerSubsystem* StageMgr = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>())
	{
		// 1->2 가는 길 
		if (Img_Path1To2 && InStageIDs.IsValidIndex(1))
		{
			const FStageClearedData& ClearedData = StageMgr->GetStageClearedData(InStageIDs[1]);
			bool bIsUnlocked = ClearedData.bIsUnlocked || StageMgr->IsCheatStageUnlocked();
			Img_Path1To2->SetColorAndOpacity(bIsUnlocked ? FLinearColor::White : FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));
		}

		// 2->3 가는 길
		if (Img_Path2To3 && InStageIDs.IsValidIndex(2))
		{
			const FStageClearedData& ClearedData = StageMgr->GetStageClearedData(InStageIDs[2]);
			bool bIsUnlocked = ClearedData.bIsUnlocked || StageMgr->IsCheatStageUnlocked();
			Img_Path2To3->SetColorAndOpacity(bIsUnlocked ? FLinearColor::White : FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));
		}

		// 3->4 가는 길
		if (Img_Path3To4 && InStageIDs.IsValidIndex(3))
		{
			const FStageClearedData& ClearedData = StageMgr->GetStageClearedData(InStageIDs[3]);
			bool bIsUnlocked = ClearedData.bIsUnlocked || StageMgr->IsCheatStageUnlocked();
			Img_Path3To4->SetColorAndOpacity(bIsUnlocked ? FLinearColor::White : FLinearColor(0.1f, 0.1f, 0.1f, 1.0f));
		}
	}
}
