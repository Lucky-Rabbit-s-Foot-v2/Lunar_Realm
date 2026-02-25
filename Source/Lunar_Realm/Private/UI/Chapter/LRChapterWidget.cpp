// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Chapter/LRStageSelectorWidget.h"

void ULRChapterWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Open) Btn_Open->OnClicked.AddDynamic(this, &ULRChapterWidget::OnOpenButtonClicked);
}

void ULRChapterWidget::UnbindProperties()
{
	if (Btn_Open) Btn_Open->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRChapterWidget::OnOpenButtonClicked()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUI<ULRStageSelectorWidget>(StageSelectorWidgetClass);
}
