// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Intro/LRTitleWidget.h"
#include "UI/Core/LRBackgroundWidget.h"

#include "Kismet/GameplayStatics.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Core/LRGameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"


void ULRTitleWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Start)
	{
		Btn_Start->OnClicked.AddDynamic(this, &ULRTitleWidget::OnClickedStartButton);
	}
}

void ULRTitleWidget::UnbindProperties()
{
	if (Btn_Start)
	{
		Btn_Start->OnClicked.Clear();
	}

	Super::UnbindProperties();
}

void ULRTitleWidget::OnClickedStartButton()
{
	ULRGameInstance* GI = Cast<ULRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (GI)
	{
		GI->OpenNextLevelByName(ELevelName::LOBBY);
	}
}
