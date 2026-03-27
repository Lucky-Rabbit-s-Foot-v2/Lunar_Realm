// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPartySlotsWidget.generated.h"

/**
 * 
 */

UCLASS()
class LUNAR_REALM_API ULRPartySlotsWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void BindToController(class ALRControllerBase* Controller) override;

	virtual void RegisterSubWidgets() override;

	UFUNCTION()
	void SetCurrentImage(const FSelectedInfo& InInfo);

	UFUNCTION()
	void OnPartyEnhanceClicked();

	UFUNCTION()
	void OnPartyReleaseClicked();

	UFUNCTION()
	void SetEnableButtons(bool bIsEnable);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Current;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Enhance;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Release;

	UPROPERTY(EditAnywhere, Category = "LR|UI Party")
	TObjectPtr<class UTexture2D> EmptySlotTexture;
};
