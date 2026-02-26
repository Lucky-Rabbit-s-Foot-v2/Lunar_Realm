// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPlayerWidget.h"
#include "UI/InGame/LRSkillPanelWidget.h"

#include "Components/Button.h"

#include "UI/InGame/LRAetherWidget.h"
#include "UI/InGame/LRHealthWidget.h"
#include "UI/InGame/LRSummonPanelWidget.h"

#include "Units/Player/LRPlayerController.h"


void ULRPlayerWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Change)
	{
		Btn_Change->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnChangeClicked);
	}
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
	LR_WARN(TEXT("Initializing Player Widget GAS with ASC: %s"), *GetNameSafe(ASC));

	if (Widget_Aether)
	{
		LR_WARN(TEXT("Initializing Player Widget GAS with ASC: %s"), *GetNameSafe(ASC));

		Widget_Aether->BindToASC(ASC);
	}

	if (Widget_HealthBar)
	{
		LR_WARN(TEXT("Initializing Player Widget GAS with ASC: %s"), *GetNameSafe(ASC));

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
