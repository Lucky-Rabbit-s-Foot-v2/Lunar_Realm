// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRSkillPanelWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260225) BJM 제작. 스킬패널용 UI
//=============================================================================

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillButtonClicked);

UCLASS()
class LUNAR_REALM_API ULRSkillPanelWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void BindToController(ALRControllerBase* Controller) override;

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
};
