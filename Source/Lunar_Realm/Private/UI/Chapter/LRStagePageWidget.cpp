// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStagePageWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "UI/Chapter/LRStageWidget.h"

#include "Units/LRControllerBase.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRStagePageWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRStagePageWidget::OnBackButtonClicked);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRStagePageWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();

	Super::UnbindProperties();
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

void ULRStagePageWidget::SetChapterID(FName InID)
{
	CurrentChapterID = InID;

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
}

void ULRStagePageWidget::OnBackButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
