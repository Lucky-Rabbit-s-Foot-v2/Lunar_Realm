// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRTitleWidget.h"

#include "Kismet/GameplayStatics.h"
#include "System/LoggingSystem.h"
#include "Components/Button.h"
#include "Core/LRGameInstance.h"

ULRTitleWidget::ULRTitleWidget()
{
	UILayer = EUILayer::PERSISTENT;
	ZOrder = 1;
}

void ULRTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Start)
	{
		Btn_Start->OnClicked.Clear();
		Btn_Start->OnClicked.AddDynamic(this, &ULRTitleWidget::OnClickedStartButton);
	}
}

void ULRTitleWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(Btn_Start)
	{
		Btn_Start->OnClicked.Clear();
	}
}

void ULRTitleWidget::OnClickedStartButton()
{
	LR_SCREEN_INFO(TEXT("Start Button Clicked"));

	ULRGameInstance* GI = Cast<ULRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		GI->SetNextLevelName(ELevelName::Lobby);
		GI->OpenNextLevel();
	}
}
