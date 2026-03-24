// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LRSettingScrollWidget.h"

#include "Engine/GameInstance.h"

#include "UI/Setting/LROptionNameButtonsWidget.h"
#include "UI/Setting/LROptionNameBarWidget.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"


void ULRSettingScrollWidget::SaveAllSettings()
{
	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OptionManager->UpdateCurrentOptionSaveGame();
	}
}

void ULRSettingScrollWidget::SetDefaultSettings()
{
	//if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	//{
	//	OptionManager->CreateNewOptionSaveData();
	//	OptionManager->InitializeOptions();
	//}
}

void ULRSettingScrollWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(TextureWidget);
	SubWidgets.Add(ShadowWidget);
	SubWidgets.Add(AntiAliasingWidget);
	SubWidgets.Add(PostProcessingWidget);
	SubWidgets.Add(VFXWidget);
	SubWidgets.Add(ResolutionScaleWidget);
	SubWidgets.Add(FrameRateWidget);

	SubWidgets.Add(MasterSoundWidget);
	SubWidgets.Add(BGMWidget);
	SubWidgets.Add(SFXWidget);
}
