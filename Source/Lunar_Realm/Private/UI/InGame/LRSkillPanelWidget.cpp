// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRSkillPanelWidget.h"
#include "Units/Player/LRPlayerController.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Components/Button.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateTypes.h"
#include "Materials/MaterialInstanceDynamic.h"


void ULRSkillPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Skill1) Btn_Skill1->OnClicked.AddUniqueDynamic(this, &ULRSkillPanelWidget::OnSkill1Clicked);
	if (Btn_Skill2) Btn_Skill2->OnClicked.AddUniqueDynamic(this, &ULRSkillPanelWidget::OnSkill2Clicked);
	if (Btn_Potion) Btn_Potion->OnClicked.AddUniqueDynamic(this, &ULRSkillPanelWidget::OnPotionClicked);
	
	if (Img_Cooldown1)
	{
		Mat_Cooldown1 = Img_Cooldown1->GetDynamicMaterial();
		if (Mat_Cooldown1) Mat_Cooldown1->SetScalarParameterValue(FName("Percent"), 0.0f);
	}
	if (Img_Cooldown2)
	{
		Mat_Cooldown2 = Img_Cooldown2->GetDynamicMaterial();
		if (Mat_Cooldown2) Mat_Cooldown2->SetScalarParameterValue(FName("Percent"), 0.0f);
	}
	if (Img_CooldownPotion)
	{
		Mat_CooldownPotion = Img_CooldownPotion->GetDynamicMaterial();
		if (Mat_CooldownPotion) Mat_CooldownPotion->SetScalarParameterValue(FName("Percent"), 0.0f);
	}
}

void ULRSkillPanelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 스킬 1번 쿨타임
	if (CurrentCD1 > 0.0f)
	{
		CurrentCD1 -= InDeltaTime;
		if (Mat_Cooldown1)
		{
			float Percent = FMath::Clamp(CurrentCD1 / MaxCD1, 0.0f, 1.0f);
			Mat_Cooldown1->SetScalarParameterValue(FName("Percent"), Percent);
		}
	}

	// 스킬 2번 쿨타임
	if (CurrentCD2 > 0.0f)
	{
		CurrentCD2 -= InDeltaTime;
		if (Mat_Cooldown2)
		{
			float Percent = FMath::Clamp(CurrentCD2 / MaxCD2, 0.0f, 1.0f);
			Mat_Cooldown2->SetScalarParameterValue(FName("Percent"), Percent);
		}
	}

	// 물약 쿨타임
	if (CurrentCDPotion > 0.0f)
	{
		CurrentCDPotion -= InDeltaTime;
		if (Mat_CooldownPotion)
		{
			float Percent = FMath::Clamp(CurrentCDPotion / MaxCDPotion, 0.0f, 1.0f);
			Mat_CooldownPotion->SetScalarParameterValue(FName("Percent"), Percent);
		}
	}
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


void ULRSkillPanelWidget::StartSkillCooldown(int32 SkillIndex, float InCooldownTime)
{
	if (SkillIndex == 1)
	{
		MaxCD1 = InCooldownTime;
		CurrentCD1 = InCooldownTime;
	}
	else if (SkillIndex == 2)
	{
		MaxCD2 = InCooldownTime;
		CurrentCD2 = InCooldownTime;
	}
}

void ULRSkillPanelWidget::OnSkill1Clicked() 
{ 
	if (CurrentCD1 > 0.0f) return; 
	OnSkill1ClickedDel.Broadcast(); 
}
void ULRSkillPanelWidget::OnSkill2Clicked() 
{ 
	if (CurrentCD2 > 0.0f) return; 
	OnSkill2ClickedDel.Broadcast(); 
}
void ULRSkillPanelWidget::OnPotionClicked() 
{ 
	if (CurrentCDPotion > 0.0f) return;

	OnPotionClickedDel.Broadcast(); 
}

void ULRSkillPanelWidget::UpdateSkillIcons(FName InPlayerSkillID, FName InWeaponSkillID)
{
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSubsystem = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSubsystem) return;

	auto ApplyIconToButtonLambda = [&](FName InSkillID, UButton* InTargetButton)
		{
			if (InSkillID == NAME_None || !InTargetButton) return;

			TSoftObjectPtr<UTexture2D> SkillIconPtr = DataSubsystem->GetSkillIcon(InSkillID);


			if (!SkillIconPtr.IsNull())
			{
				// 동기 로드
				if (UTexture2D* LoadedIcon = SkillIconPtr.LoadSynchronous())
				{
					FButtonStyle NewStyle = InTargetButton->GetStyle();

					NewStyle.Normal.SetResourceObject(LoadedIcon);
					NewStyle.Hovered.SetResourceObject(LoadedIcon);
					NewStyle.Pressed.SetResourceObject(LoadedIcon);

					InTargetButton->SetStyle(NewStyle);
				}
			}

			//// 스킬 데이터에서 리소스 ID 추출
			//const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(InSkillID);
			//FName ResourceID = SkillData.ResourceID;

			//if (ResourceID == NAME_None) return;

			//// 리소스 ID로 리소스 데이터(이미지) 추출
			//const FSkillResourceData& ResourceData = DataSubsystem->GetSkillResourceData(ResourceID);

			//

			//// 버튼 자체의 스타일에 이미지 덮어쓰기
			//if (!ResourceData.SkillIcon.IsNull())
			//{
			//	UTexture2D* LoadedIcon = ResourceData.SkillIcon.LoadSynchronous();
			//	if (LoadedIcon)
			//	{
			//		FButtonStyle NewStyle = InTargetButton->GetStyle();

			//		NewStyle.Normal.SetResourceObject(LoadedIcon);
			//		NewStyle.Hovered.SetResourceObject(LoadedIcon);
			//		NewStyle.Pressed.SetResourceObject(LoadedIcon);

			//		InTargetButton->SetStyle(NewStyle);
			//	}
			//}
		};

	// 플레이어 스킬(1번)과 무기 스킬(2번) 버튼에 각각 람다 함수 실행
	ApplyIconToButtonLambda(InPlayerSkillID, Btn_Skill1);
	ApplyIconToButtonLambda(InWeaponSkillID, Btn_Skill2);
}

void ULRSkillPanelWidget::StartPotionCooldown(float InCooldownTime)
{
	MaxCDPotion = InCooldownTime;
	CurrentCDPotion = InCooldownTime;
}
