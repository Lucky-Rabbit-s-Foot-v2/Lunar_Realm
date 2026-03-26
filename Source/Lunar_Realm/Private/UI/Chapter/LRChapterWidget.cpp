// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "UI/Chapter/LRStagePageWidget.h"
#include "UI/Chapter/LRChapterPageWidget.h"

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

	if (ULRChapterPageWidget* ParentPage = GetTypedOuter<ULRChapterPageWidget>())
	{
		ParentPage->PlayFadeOutAndOpenStage(ChapterID);
	}
	else
	{
		if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
		{
			if (ULRStagePageWidget* StagePage = Cast<ULRStagePageWidget>(UIManager->OpenUIByID(EUIID::STAGE)))
			{
				StagePage->SetChapterID(ChapterID);
			}
		}
	}

	//const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	//UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();

	//ULRStagePageWidget* StagePage = Cast<ULRStagePageWidget>(UIManager->OpenUIByID(EUIID::STAGE));
	//StagePage->SetChapterID(ChapterID);

}
