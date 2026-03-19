// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPersistentWidget.h"
#include "LRInGamePersistentWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260204) BJM 제작. 인게임 관련 Widget 생성.
// (260209) BJM OpenUI Visible -> SelfHitTestInvisible 로 변경
// (260223) PJB InGame UI 연동
// (260225) BJM SkillPanel 생성으로 인한 코드 정리
//=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpeedClicked);

class ULRSkillPanelWidget;
class URetainerBox;

UCLASS()
class LUNAR_REALM_API ULRInGamePersistentWidget : public ULRPersistentWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RegisterSubWidgets() override;

	virtual void InitializeUI() override;
	virtual void OpenUI() override;

	UFUNCTION(BlueprintCallable)
	void TestSummonPanelRefresh();

	FOnChangeClicked OnChangeClickedDel;

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void InitializeGAS(UAbilitySystemComponent* ASC);
	
	virtual void BindToController(class ALRControllerBase* Controller) override;

	UFUNCTION(BlueprintCallable)
	void OnPauseButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "LR|UI")

	void UpdateUIOnDeath(bool InIsDead, float InRespawnTime = 0.0f);

private:
	
	UFUNCTION(BlueprintCallable)
	void OnChangeClicked();

public:
	void RefreshSkillPanelIcons(FName InPlayerSkillID, FName InWeaponSkillID);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRSkillPanelWidget> WBP_SkillPanel;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Change;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRAetherWidget> Widget_Aether;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRHealthWidget> Widget_HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRSummonPanelWidget> WBP_SummonPanel;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Pause;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URetainerBox> Retainer_SkillPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class URetainerBox> Retainer_HealthBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TObjectPtr<class UMaterialInterface> Mat_BlackWhite;

	// 오토모드 제어용
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Auto;

	UPROPERTY(EditDefaultsOnly, Category = "UI|AutoMode")
	class UTexture2D* Tex_AutoOn;

	UPROPERTY(EditDefaultsOnly, Category = "UI|AutoMode")
	class UTexture2D* Tex_AutoOff;

public:
	void UpdateAutoButtonVisual(bool InbIsAutoMode);

protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_AutoGlow;

public:
	void SetAutoEffectActive(bool bIsActive);

// 시간 배수 옵션 버튼
public:
	FOnSpeedClicked OnSpeedClickedDel;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateSpeedVisual(float InCurrentSpeed);

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void SetSpeedButtonLocked(bool InbIsLocked);

private:
	UFUNCTION(BlueprintCallable)
	void OnSpeedClicked();

protected:
	UPROPERTY(meta = (Bindwidget))
	TObjectPtr<class UButton> Btn_Speed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_SpeedLock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_SpeedGlow;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_SpeedArrow;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Speed")
	class UTexture2D* Tex_Speed1; // 1배속 (화살표 1개)

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Speed")
	class UTexture2D* Tex_Speed2; // 1.5배속 (화살표 2개)

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Speed")
	class UTexture2D* Tex_Speed3; // 2배속 (화살표 3개)


};
