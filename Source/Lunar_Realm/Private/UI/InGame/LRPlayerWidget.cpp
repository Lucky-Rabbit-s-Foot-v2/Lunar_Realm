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

	//if (Btn_Skill1)
	//{
	//	Btn_Skill1->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnSkill1Clicked);
	//}

	//if (Btn_Skill2)
	//{
	//	Btn_Skill2->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnSkill2Clicked);
	//}

	//if (Btn_Potion)
	//{
	//	Btn_Potion->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnPotionClicked);
	//}

	if (Btn_Change)
	{
		Btn_Change->OnClicked.AddDynamic(this, &ULRPlayerWidget::OnChangeClicked);
	}
}

void ULRPlayerWidget::NativeDestruct()
{
	Btn_Change->OnClicked.Clear();
	//Btn_Potion->OnClicked.Clear();
	//Btn_Skill2->OnClicked.Clear();
	//Btn_Skill1->OnClicked.Clear();

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


//void ULRPlayerWidget::OnSkill1Clicked()
//{
//	OnSkill1ClickedDel.Broadcast();
//}
//
//void ULRPlayerWidget::OnSkill2Clicked()
//{
//	OnSkill2ClickedDel.Broadcast();
//}
//
//void ULRPlayerWidget::OnPotionClicked()
//{
//	OnPotionClickedDel.Broadcast();
//}

void ULRPlayerWidget::OnChangeClicked()
{
	OnChangeClickedDel.Broadcast();
}
