// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/StageWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/StageManagerSubsystem.h"

void UStageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Open)
	{
		Btn_Open->OnClicked.AddDynamic(this, &UStageWidget::OnOpenButtonClicked);
	}

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		OnStageOpenClickedDel.AddDynamic(GI, &ULRGameInstance::OpenNextStage);
	}
}

void UStageWidget::NativeDestruct()
{
	OnStageOpenClickedDel.Clear();
	
	if (Btn_Open)
	{
		Btn_Open->OnClicked.Clear();
	}
	Super::NativeDestruct();
}

void UStageWidget::OpenUI()
{
	Super::OpenUI();
}

void UStageWidget::CloseUI()
{
	Super::CloseUI();
}

void UStageWidget::RefreshUI()
{
	Super::RefreshUI();
}

void UStageWidget::OnOpenButtonClicked()
{
	OnStageOpenClickedDel.Broadcast(StageID);
}