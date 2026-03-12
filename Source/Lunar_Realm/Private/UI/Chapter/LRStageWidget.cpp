// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Chapter/LRStageReadyWidget.h"

void ULRStageWidget::OnOpenButtonClicked()
{
	UUIManagerSubsystem* UIManagerSubsystem = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	ULRStageReadyWidget* Widget = UIManagerSubsystem->OpenUI<ULRStageReadyWidget>(StageReadyClass);
	Widget->SetStageID(StageID);
}

void ULRStageWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Open) Btn_Open->OnClicked.AddDynamic(this, &ULRStageWidget::OnOpenButtonClicked);
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

		Txt_Name->SetText(StageData.StageName);
	}
}

void ULRStageWidget::SetStageID(FName InStageID)
{
	StageID = InStageID;
	
	LR_INFO(TEXT("StageID set to %s in ULRStageWidget::SetStageID"), *StageID.ToString());

	RefreshUI();
}
