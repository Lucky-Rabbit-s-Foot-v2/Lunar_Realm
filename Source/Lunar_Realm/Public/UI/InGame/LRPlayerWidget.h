// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LRPersistentWidget.h"
#include "LRPlayerWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260204) BJM 제작. 인게임 관련 Widget 생성.
// (260209_BJM) OpenUI Visible -> SelfHitTestInvisible 로 변경
// (260223) PJB InGame UI 연동
//=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkill1Clicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkill2Clicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPotionClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeClicked);

UCLASS()
class LUNAR_REALM_API ULRPlayerWidget : public ULRPersistentWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void InitializeUI() override;
	virtual void OpenUI() override;

	UFUNCTION(BlueprintCallable)
	void TestSummonPanelRefresh();

	FOnSkill1Clicked OnSkill1ClickedDel;
	FOnSkill2Clicked OnSkill2ClickedDel;
	FOnPotionClicked OnPotionClickedDel;
	FOnChangeClicked OnChangeClickedDel;

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void InitializeGAS(UAbilitySystemComponent* ASC);
	
	virtual void BindToController(class ALRControllerBase* Controller) override;

private:
	UFUNCTION(BlueprintCallable)
	void OnSkill1Clicked();

	UFUNCTION(BlueprintCallable)
	void OnSkill2Clicked();
	
	UFUNCTION(BlueprintCallable)
	void OnPotionClicked();
	
	UFUNCTION(BlueprintCallable)
	void OnChangeClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Skill1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Skill2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Potion;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Change;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRAetherWidget> Widget_Aether;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRHealthWidget> Widget_HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRSummonPanelWidget> WBP_SummonPanel;

};
