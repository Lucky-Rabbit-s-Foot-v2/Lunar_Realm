// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRPartySlotsWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRPartySlotsWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;
	
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void BindSubWidgets() override;
	virtual void RegisterSubWidgets() override;

	UFUNCTION()
	void RefreshPartySlots();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Current;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot_Main;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotWidget> Slot_4;
};
