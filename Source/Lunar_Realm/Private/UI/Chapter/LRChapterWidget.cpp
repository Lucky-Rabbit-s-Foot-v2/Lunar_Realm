// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Chapter/LRStagePageWidget.h"

void ULRChapterWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Open) Btn_Open->OnClicked.AddDynamic(this, &ULRChapterWidget::OnOpenButtonClicked);
}

void ULRChapterWidget::UnbindProperties()
{
	if (Btn_Open) Btn_Open->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRChapterWidget::RefreshUI()
{
	Super::RefreshUI();

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const FChapterStaticData& ChapterData = GameDataSubsystem->GetChapterStaticData(ChapterID);
	
	Img_Icon->SetBrushFromTexture(ChapterData.ChapterThumbnail.LoadSynchronous());
	Txt_Name->SetText(ChapterData.ChapterName);
}

void ULRChapterWidget::SetChapterID(FName InChapterID)
{
	ChapterID = InChapterID;
	RefreshUI();
}

void ULRChapterWidget::OnOpenButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	ULRStagePageWidget* StageSelectorWidget = UIManager->OpenUI<ULRStagePageWidget>(StageSelectorWidgetClass);

	StageSelectorWidget->SetChapterID(ChapterID);

	UIManager->OpenUI<ULRStagePageWidget>(StageSelectorWidgetClass);
}
