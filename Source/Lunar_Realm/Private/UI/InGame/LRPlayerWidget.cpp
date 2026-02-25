// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPlayerWidget.h"
#include "UI/InGame/LRSkillPanelWidget.h"

#include "Components/Button.h"

#include "UI/InGame/LRAetherWidget.h"
#include "UI/InGame/LRHealthWidget.h"
#include "UI/InGame/LRSummonPanelWidget.h"

#include "Units/Player/LRPlayerController.h"


void ULRPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Change)
	{
		Btn_Change->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnChangeClicked);
	}
}

void ULRPlayerWidget::NativeDestruct()
{
	Btn_Change->OnClicked.Clear();

	Super::NativeDestruct();
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
