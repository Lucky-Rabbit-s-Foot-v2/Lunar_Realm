// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Setting/LRSettingScrollWidget.h"

#include "Engine/GameInstance.h"

#include "UI/Common/LRNameButtonsWidget.h"
#include "UI/Common/LRNameBarWidget.h"

#include "Subsystems/Option/OptionManagerSubsystem.h"


void ULRSettingScrollWidget::InitializeUI()
{
	Super::InitializeUI();

	TextureWidget->SetName(FText::FromString(TEXT("텍스쳐")));
	ShadowWidget->SetName(FText::FromString(TEXT("그림자")));
	AntiAliasingWidget->SetName(FText::FromString(TEXT("안티앨리어싱")));
	PostProcessingWidget->SetName(FText::FromString(TEXT("포스트 프로세싱")));
	VFXWidget->SetName(FText::FromString(TEXT("VFX")));

	ResolutionScaleWidget->SetName(FText::FromString(TEXT("해상도 스케일")));
	FrameRateWidget->SetName(FText::FromString(TEXT("프레임 레이트")));
	
	MasterSoundWidget->SetName(FText::FromString(TEXT("마스터 볼륨")));
	BGMWidget->SetName(FText::FromString(TEXT("BGM 볼륨")));
	SFXWidget->SetName(FText::FromString(TEXT("SFX 볼륨")));

	SetDefaultSettings();
}

void ULRSettingScrollWidget::SetDefaultSettings()
{
	// TODO: 각 설정 위젯에 기본값 설정
}

void ULRSettingScrollWidget::SaveAllSettings()
{
	if (UOptionManagerSubsystem* OptionManager = GetGameInstance()->GetSubsystem<UOptionManagerSubsystem>())
	{
		OptionManager->UpdateCurrentOptionSaveGame();
	}
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
