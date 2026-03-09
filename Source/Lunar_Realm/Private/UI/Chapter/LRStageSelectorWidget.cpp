// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageSelectorWidget.h"

#include "Components/Button.h"
#include "UI/Chapter/LRStageWidget.h"

#include "Units/LRControllerBase.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

void ULRStageSelectorWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRStageSelectorWidget::OnBackButtonClicked);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRStageSelectorWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRStageSelectorWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Stage1);
	SubWidgets.Add(Stage2);
	SubWidgets.Add(Stage3);
	SubWidgets.Add(Stage4);
	SubWidgets.Add(Stage5);
}

void ULRStageSelectorWidget::SetStageData(const TArray<FName>& StageIDs)
{
	for (int32 i = 0; i < StageIDs.Num() && i < StageWidgets.Num(); ++i)
	{
		if (StageWidgets[i])
		{
			StageWidgets[i]->SetStageID(StageIDs[i]);
		}
	}
}

void ULRStageSelectorWidget::OnBackButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
}
