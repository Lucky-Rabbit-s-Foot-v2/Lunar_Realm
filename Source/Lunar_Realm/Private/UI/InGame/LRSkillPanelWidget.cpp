// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRSkillPanelWidget.h"
#include "Units/Player/LRPlayerController.h"
#include "Subsystems/GameDataSubsystem.h"

#include "Components/Button.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateTypes.h"


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

void ULRSkillPanelWidget::UpdateSkillIcons(FName InPlayerSkillID, FName InWeaponSkillID)
{
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSubsystem = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSubsystem) return;

	// 중복 방지용 람다 헬퍼 함수: 스킬 ID랑 버튼을 주면 버튼 스타일을 바꿔줌
	auto ApplyIconToButtonLambda = [&](FName InSkillID, UButton* InTargetButton)
		{
			if (InSkillID == NAME_None || !InTargetButton) return;

			// 1. 스킬 데이터에서 리소스 ID 추출
			const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(InSkillID);
			FName ResourceID = SkillData.ResourceID;

			if (ResourceID == NAME_None) return;

			// 2. 리소스 ID로 리소스 데이터(이미지) 추출
			const FSkillResourceData& ResourceData = DataSubsystem->GetSkillResourceData(ResourceID);

			// 3. 버튼 자체의 스타일에 이미지 덮어쓰기
			if (!ResourceData.SkillIcon.IsNull())
			{
				UTexture2D* LoadedIcon = ResourceData.SkillIcon.LoadSynchronous();
				if (LoadedIcon)
				{
					// ★ 여기가 핵심! 버튼의 현재 스타일을 복사해옴
					FButtonStyle NewStyle = InTargetButton->GetStyle();

					// 평상시, 마우스 오버, 클릭 시의 이미지를 전부 로드한 아이콘으로 변경
					NewStyle.Normal.SetResourceObject(LoadedIcon);
					NewStyle.Hovered.SetResourceObject(LoadedIcon);
					NewStyle.Pressed.SetResourceObject(LoadedIcon);

					// (선택) 만약 버튼 크기에 맞게 꽉 채우고 싶다면 이미지 사이즈를 세팅할 수도 있어!
					// NewStyle.Normal.ImageSize = FVector2D(LoadedIcon->GetSizeX(), LoadedIcon->GetSizeY());

					// 수정한 스타일을 버튼에 최종 적용!
					InTargetButton->SetStyle(NewStyle);
				}
			}
		};

	// 플레이어 스킬(1번)과 무기 스킬(2번) 버튼에 각각 람다 함수 실행
	ApplyIconToButtonLambda(InPlayerSkillID, Btn_Skill1);
	ApplyIconToButtonLambda(InWeaponSkillID, Btn_Skill2);
}
