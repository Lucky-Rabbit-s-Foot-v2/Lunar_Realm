// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRPartySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRPartySlotWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshUI() override;

	UFUNCTION()
	void OnSlotButtonClicked();

	void SetCharacterID(FName InID) { ID = InID; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Slot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Slot;

	UPROPERTY(EditAnywhere, Category = "LR|UI Party")
	int32 SlotIndex = 0;

private:
	FName ID;
};
