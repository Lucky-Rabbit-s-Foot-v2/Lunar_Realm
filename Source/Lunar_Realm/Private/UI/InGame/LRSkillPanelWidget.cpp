// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRSkillPanelWidget.h"
#include "Components/Button.h"
#include "Units/Player/LRPlayerController.h"

void ULRSkillPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Skill1) Btn_Skill1->OnClicked.AddUniqueDynamic(this, &ULRSkillPanelWidget::OnSkill1Clicked);
	if (Btn_Skill2) Btn_Skill2->OnClicked.AddUniqueDynamic(this, &ULRSkillPanelWidget::OnSkill2Clicked);
	if (Btn_Potion) Btn_Potion->OnClicked.AddUniqueDynamic(this, &ULRSkillPanelWidget::OnPotionClicked);
}

void ULRSkillPanelWidget::NativeDestruct()
{
	if (Btn_Skill1) Btn_Skill1->OnClicked.Clear();
	if (Btn_Skill2) Btn_Skill2->OnClicked.Clear();
	if (Btn_Potion) Btn_Potion->OnClicked.Clear();

	Super::NativeDestruct();
}

void ULRSkillPanelWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	ALRPlayerController* PC = Cast<ALRPlayerController>(Controller);
	if (PC)
	{
		OnPotionClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::UsePotion);
		OnSkill1ClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::UseSkill1);
		OnSkill2ClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::UseSkill2);
	}
}

void ULRSkillPanelWidget::OnSkill1Clicked() 
{ 
	OnSkill1ClickedDel.Broadcast(); 
}
void ULRSkillPanelWidget::OnSkill2Clicked() 
{ 
	OnSkill2ClickedDel.Broadcast(); 
}
void ULRSkillPanelWidget::OnPotionClicked() 
{ 
	OnPotionClickedDel.Broadcast(); 
}