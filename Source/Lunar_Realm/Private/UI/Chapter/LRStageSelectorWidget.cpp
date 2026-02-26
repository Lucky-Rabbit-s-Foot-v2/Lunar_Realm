// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageSelectorWidget.h"

#include "Components/Button.h"
#include "UI/Chapter/LRStageWidget.h"

#include "Units/LRControllerBase.h"

void ULRStageSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StageWidgets.Empty();
	StageWidgets.Add(Stage1);
	StageWidgets.Add(Stage2);
	StageWidgets.Add(Stage3);
	StageWidgets.Add(Stage4);
	StageWidgets.Add(Stage5);
}

void ULRStageSelectorWidget::NativeDestruct()
{
	StageWidgets.Empty();

	Super::NativeDestruct();
}

void ULRStageSelectorWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRStageSelectorWidget::OnBackButtonClicked);
}

void ULRStageSelectorWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRStageSelectorWidget::RefreshUI()
{
	Super::RefreshUI();

	for (auto& StageWidget : StageWidgets)
	{
		if (StageWidget)
		{
			StageWidget->RefreshUI();
		}
	}
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
