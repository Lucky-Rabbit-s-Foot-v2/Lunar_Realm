// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Components/Image.h"
#include "LRSkillPanelWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260225) BJM 제작. 스킬패널용 UI
//=============================================================================

class UButton;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillButtonClicked);

UCLASS()
class LUNAR_REALM_API ULRSkillPanelWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void BindToController(ALRControllerBase* Controller) override;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void StartSkillCooldown(int32 SkillIndex, float InCooldownTime);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSkillButtonClicked OnSkill1ClickedDel;

	UPROPERTY(BlueprintAssignable)
	FOnSkillButtonClicked OnSkill2ClickedDel;

	UPROPERTY(BlueprintAssignable)
	FOnSkillButtonClicked OnPotionClickedDel;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Skill1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Skill2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Potion;

	UFUNCTION()
	void OnSkill1Clicked();

	UFUNCTION()
	void OnSkill2Clicked();

	UFUNCTION()
	void OnPotionClicked();

public:
	void UpdateSkillIcons(FName InPlayerSkillID, FName InWeaponSkillID);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* Img_Cooldown1;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Cooldown2;

	UPROPERTY()
	class UMaterialInstanceDynamic* Mat_Cooldown1;

	UPROPERTY()
	class UMaterialInstanceDynamic* Mat_Cooldown2;

	// 쿨타임 시간 추적용 변수
	float MaxCD1 = 0.0f;
	float CurrentCD1 = 0.0f;
	float MaxCD2 = 0.0f;
	float CurrentCD2 = 0.0f;



};
