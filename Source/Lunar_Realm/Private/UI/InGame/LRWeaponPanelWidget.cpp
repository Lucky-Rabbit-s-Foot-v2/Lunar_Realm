// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRWeaponPanelWidget.h"
#include "Components/Button.h"
#include "Units/Player/LRPlayerController.h"
#include "System/LoggingSystem.h"



void ULRWeaponPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Weapon1) Btn_Weapon1->OnClicked.AddUniqueDynamic(this, &ULRWeaponPanelWidget::OnWeapon1Clicked);
	if (Btn_Weapon2) Btn_Weapon2->OnClicked.AddUniqueDynamic(this, &ULRWeaponPanelWidget::OnWeapon2Clicked);
	if (Btn_Weapon3) Btn_Weapon3->OnClicked.AddUniqueDynamic(this, &ULRWeaponPanelWidget::OnWeapon3Clicked);
	if (Btn_Unequip) Btn_Unequip->OnClicked.AddUniqueDynamic(this, &ULRWeaponPanelWidget::OnUnequipClicked);

	LR_INFO(TEXT("[LRWeaponPanelWidget] 무기 패널 생성 및 버튼 바인딩 완료!"));
}

void ULRWeaponPanelWidget::NativeDestruct()
{
	if (Btn_Weapon1) Btn_Weapon1->OnClicked.Clear();
	if (Btn_Weapon2) Btn_Weapon2->OnClicked.Clear();
	if (Btn_Weapon3) Btn_Weapon3->OnClicked.Clear();
	if (Btn_Unequip) Btn_Unequip->OnClicked.Clear();

	Super::NativeDestruct();
}

void ULRWeaponPanelWidget::BindToController(ALRControllerBase* InController)
{
	ALRPlayerController* PC = Cast<ALRPlayerController>(InController);
	if (PC)
	{
		 OnWeapon1ClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::EquipWeapon1);
		 OnWeapon2ClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::EquipWeapon2);
		 OnWeapon3ClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::EquipWeapon3);
		 OnUnequipClickedDel.AddUniqueDynamic(PC, &ALRPlayerController::UnequipWeapon);

		LR_INFO(TEXT("[LRWeaponPanelWidget] 플레이어 컨트롤러 바인딩 준비 완료"));
	}
}

void ULRWeaponPanelWidget::OnWeapon1Clicked()
{
	LR_INFO(TEXT("[LRWeaponPanelWidget] 무기 1번 교체 버튼 클릭됨"));
	OnWeapon1ClickedDel.Broadcast();
}

void ULRWeaponPanelWidget::OnWeapon2Clicked()
{
	LR_INFO(TEXT("[LRWeaponPanelWidget] 무기 2번 교체 버튼 클릭됨"));
	OnWeapon2ClickedDel.Broadcast();
}

void ULRWeaponPanelWidget::OnWeapon3Clicked()
{
	LR_INFO(TEXT("[LRWeaponPanelWidget] 무기 3번 교체 버튼 클릭됨"));
	OnWeapon3ClickedDel.Broadcast();
}

void ULRWeaponPanelWidget::OnUnequipClicked()
{
	LR_INFO(TEXT("[LRWeaponPanelWidget] 무기 장착 해제 버튼 클릭됨"));
	OnUnequipClickedDel.Broadcast();
}
