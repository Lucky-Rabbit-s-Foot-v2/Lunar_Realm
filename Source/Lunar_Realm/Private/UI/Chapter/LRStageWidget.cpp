// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRStageWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Core/LRGameInstance.h"
#include "Subsystems/StageManagerSubsystem.h"

void ULRStageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Open)
	{
		Btn_Open->OnClicked.AddDynamic(this, &ULRStageWidget::OnOpenButtonClicked);
	}

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetWorld()->GetGameInstance()))
	{
		OnStageOpenClickedDel.AddDynamic(GI, &ULRGameInstance::OpenNextStage);
	}
}

void ULRStageWidget::NativeDestruct()
{
	OnStageOpenClickedDel.Clear();
	
	if (Btn_Open)
	{
		Btn_Open->OnClicked.Clear();
	}
	Super::NativeDestruct();
}

void ULRStageWidget::OnOpenButtonClicked()
{
	OnStageOpenClickedDel.Broadcast(StageID);
}