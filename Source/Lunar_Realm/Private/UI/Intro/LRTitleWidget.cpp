// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRTitleWidget.h"

#include "Kismet/GameplayStatics.h"
#include "System/LoggingSystem.h"
#include "Components/Button.h"
#include "Core/LRGameInstance.h"

void ULRTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
	{
		OnChangeLevelRequestedDel.AddDynamic(GI, &ULRGameInstance::OpenNextLevel);
	}

	if (Btn_Start)
	{
		Btn_Start->OnClicked.AddDynamic(this, &ULRTitleWidget::OnClickedStartButton);
	}
}

void ULRTitleWidget::NativeDestruct()
{
	OnChangeLevelRequestedDel.Clear();

	if(Btn_Start)
	{
		Btn_Start->OnClicked.Clear();
	}

	Super::NativeDestruct();
}

void ULRTitleWidget::OnClickedStartButton()
{
	ULRGameInstance* GI = Cast<ULRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		GI->SetNextLevelName(ELevelName::LOBBY);
		OnChangeLevelRequestedDel.Broadcast();
	}
}
