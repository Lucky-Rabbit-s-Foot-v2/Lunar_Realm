// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRTitleWidget.h"

#include "Kismet/GameplayStatics.h"

#include "Components/Button.h"

#include "Core/LRGameInstance.h"

void ULRTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Start)
	{
		Btn_Start->OnClicked.AddDynamic(this, &ULRTitleWidget::OnClickedStartButton);
	}
}

void ULRTitleWidget::NativeDestruct()
{
	if (Btn_Start)
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
		GI->OpenNextLevelByName(ELevelName::LOBBY);
	}
}
