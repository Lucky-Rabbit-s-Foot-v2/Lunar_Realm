// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LRChapterPageWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Settings/UIManagerSettings.h"

#include "UI/Chapter/LRChapterWidget.h"

void ULRChapterPageWidget::InitializeUI()
{
	Super::InitializeUI();
	Chapter1->SetChapterID(FName("LAKE"));
	Chapter2->SetChapterID(FName("OCEAN"));
	Chapter3->SetChapterID(FName("DESERT"));
}

void ULRChapterPageWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	
	ChapterWidgets.Add(Chapter1);
	ChapterWidgets.Add(Chapter2);
	ChapterWidgets.Add(Chapter3);
}
