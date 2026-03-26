// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPartySlotsWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESelectedButtonType : uint8
{
	NONE,
	MOUNT,
	SWAP,
	CLEAR
};

UCLASS()
class LUNAR_REALM_API ULRPartySlotsWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RegisterSubWidgets() override;


	UFUNCTION()
	void OnPartyMountClicked();

	UFUNCTION()
	void OnPartySwapClicked();

	UFUNCTION()
	void OnPartyReleaseClicked();

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
	TObjectPtr<class ULRButtonWidget> Btn_Mount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Swap;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Release;

private:
	FName SelectedID = NAME_None;
	ESelectedButtonType ButtonType = ESelectedButtonType::NONE;
};
