// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPlayerWidget.h"
#include "UI/InGame/LRSkillPanelWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Core/Stage/LRStageGameMode.h"

#include "UI/InGame/LRAetherWidget.h"
#include "UI/InGame/LRHealthWidget.h"
#include "UI/InGame/LRSummonPanelWidget.h"

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerState.h"

#include "Components/RetainerBox.h"
#include "Engine/Texture2D.h"

void ULRPlayerWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Change) Btn_Change->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnChangeClicked);
	if (Btn_Pause) Btn_Pause->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnPauseButtonClicked);
}


void ULRPlayerWidget::UnbindProperties()
{
	Btn_Change->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRPlayerWidget::InitializeUI()
{
	Super::InitializeUI();

	WBP_SummonPanel->InitPanel();
}

void ULRPlayerWidget::OpenUI()
{
	Super::OpenUI();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void ULRPlayerWidget::TestSummonPanelRefresh()
{
	WBP_SummonPanel->InitPanel();
}

void ULRPlayerWidget::InitializeGAS(UAbilitySystemComponent* ASC)
{
	if (Widget_Aether)
	{
		Widget_Aether->BindToASC(ASC);
	}

	if (Widget_HealthBar)
	{
		Widget_HealthBar->BindToASC(ASC);
	}
}

void ULRPlayerWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALRPlayerController* PC = Cast<ALRPlayerController>(Controller);
	//OnPotionClickedDel.AddDynamic(PC, &ALRPlayerController::UsePotion);

	if (PC)
	{
		OnChangeClickedDel.AddDynamic(PC, &ALRPlayerController::ToggleAutoMode);
		InitializeGAS(PC->GetAbilitySystemComponent());
		
		if (ALRPlayerState* PS = PC->GetPlayerState<ALRPlayerState>())
		{
			if (Widget_HealthBar)
			{
				Widget_HealthBar->UpdatePlayerIcon(PS->GetCharacterID());
			}
		}

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = true;
	}
	if (WBP_SkillPanel)
	{
		WBP_SkillPanel->BindToController(Controller);
	}
}

void ULRPlayerWidget::OnChangeClicked()
{
	OnChangeClickedDel.Broadcast();
}


void ULRPlayerWidget::OnPauseButtonClicked()
{
	ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(UGameplayStatics::GetGameMode(this));
	StageGM->OnOpenPauseUI();
}


void ULRPlayerWidget::UpdateUIOnDeath(bool InIsDead, float InRespawnTime)
{
	UMaterialInterface* TargetMat = InIsDead ? Mat_BlackWhite.Get() : nullptr;

	if (Retainer_SkillPanel) Retainer_SkillPanel->SetEffectMaterial(TargetMat);
	if (Retainer_HealthBar) Retainer_HealthBar->SetEffectMaterial(TargetMat);

	if (WBP_SkillPanel) WBP_SkillPanel->SetIsEnabled(!InIsDead);

	if (Btn_Change) Btn_Change->SetIsEnabled(true);
	if (Btn_Pause) Btn_Pause->SetIsEnabled(true);
	if (WBP_SummonPanel) WBP_SummonPanel->SetIsEnabled(true);

	if (Widget_HealthBar)
	{
		if (InIsDead)
		{
			Widget_HealthBar->StartRespawnTimer(InRespawnTime);
		}
		else
		{
			Widget_HealthBar->StopRespawnTimer();
		}
	}
}

void ULRPlayerWidget::RefreshSkillPanelIcons(FName InPlayerSkillID, FName InWeaponSkillID)
{
	if (WBP_SkillPanel)
	{
		WBP_SkillPanel->UpdateSkillIcons(InPlayerSkillID, InWeaponSkillID);
	}
}

void ULRPlayerWidget::UpdateAutoButtonVisual(bool InbIsAutoMode)
{
	if (Btn_Change)
	{
		FButtonStyle NewButtonStyle = Btn_Change->GetStyle();
		UTexture2D* TargetTex = InbIsAutoMode ? Tex_AutoOn : Tex_AutoOff;

		if (TargetTex)
		{
			NewButtonStyle.Normal.SetResourceObject(TargetTex);
			NewButtonStyle.Hovered.SetResourceObject(TargetTex);
			NewButtonStyle.Pressed.SetResourceObject(TargetTex);
			Btn_Change->SetStyle(NewButtonStyle);
		}
	}

	if (Txt_Auto)
	{
		FSlateFontInfo FontInfo = Txt_Auto->GetFont();
		FontInfo.OutlineSettings.OutlineSize = 2; 

		if (InbIsAutoMode)
		{
			Txt_Auto->SetColorAndOpacity(FSlateColor(FLinearColor::White));

			FontInfo.OutlineSettings.OutlineColor = FLinearColor(0.0f, 0.7f, 1.0f, 1.0f);
		}
		else
		{
			Txt_Auto->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.4f)));

			FontInfo.OutlineSettings.OutlineColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.4f);
		}

		Txt_Auto->SetFont(FontInfo);
	}
}
