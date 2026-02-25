// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRSummonPanelWidget.generated.h"

class ULRSummonSlotWidget;
class UHorizontalBox;

/**
 * 
 */
 // =============================================================================
 // (260216) BJM 제작. 소환 슬롯 패널
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRSummonPanelWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;
	virtual void RefreshUI() override;

	UFUNCTION(BlueprintCallable)
	void InitPanel();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRSummonSlotWidget> SummonSlot_0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRSummonSlotWidget> SummonSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRSummonSlotWidget> SummonSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRSummonSlotWidget> SummonSlot_3;

private:
	TArray<ULRSummonSlotWidget*> SlotArray;

public:
	virtual void OpenUI() override;

};
