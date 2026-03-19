// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Chapter/LRReadyPopupWidget.h"

void ULRStageWidget::OnOpenButtonClicked()
{
	const UUIManagerSettings* Settings = GetDefault<UUIManagerSettings>();
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();

	ULRReadyPopupWidget* ReadyPopup = Cast<ULRReadyPopupWidget>(UIManager->OpenUIByID(EUIID::READY));
	ReadyPopup->SetStageID(StageID);
}

void ULRStageWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Open) Btn_Open->OnClicked.AddUniqueDynamic(this, &ULRStageWidget::OnOpenButtonClicked);
}

void ULRStageWidget::UnbindProperties()
{
	if (Btn_Open) Btn_Open->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRStageWidget::RefreshUI()
{
	Super::RefreshUI();

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		UGameDataSubsystem* GameDataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
		const FStageStaticData& StageData = GameDataSubsystem->GetStageStaticData(StageID);

		UStageManagerSubsystem* StageMgr = GI->GetSubsystem<UStageManagerSubsystem>();
		const FStageClearedData& ClearedData = StageMgr->GetStageClearedData(StageID);

		Txt_Name->SetText(StageData.StageName);

		Img_Star1->SetBrushFromTexture((ClearedData.StarMasking & 0b001) ? StarOnTexture : StarOffTexture);
		Img_Star2->SetBrushFromTexture((ClearedData.StarMasking & 0b010) ? StarOnTexture : StarOffTexture);
		Img_Star3->SetBrushFromTexture((ClearedData.StarMasking & 0b100) ? StarOnTexture : StarOffTexture);

		if (ClearedData.bIsUnlocked)
		{
			Btn_Open->SetIsEnabled(true);
			Img_Base->SetBrushFromTexture(BaseOnTexture);
		}
		else
		{
			Btn_Open->SetIsEnabled(false);
			Img_Base->SetBrushFromTexture(BaseOffTexture);
		}
	}
}

void ULRStageWidget::SetStageID(FName InStageID)
{
	StageID = InStageID;
	RefreshUI();
}
