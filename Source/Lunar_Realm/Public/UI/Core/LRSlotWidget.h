// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRSlotWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClicked, ULRSlotWidget*, InWidget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotToggled, bool, InIsSelected);

UCLASS()
class LUNAR_REALM_API ULRSlotWidget : public ULRButtonWidget
{
	GENERATED_BODY()

public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshUI() override;

	virtual void BindToController(class ALRControllerBase* Controller) override;

	virtual void SetSlotIndex(int32 InIndex);
	virtual void SetID(FName InID);
	virtual void SetGradeImage();
	virtual void SetIconImage();

	virtual void SetSelected(bool bSelected);

	int32 GetSlotIndex() const { return SlotIndex; }
	ECollectionType GetType() const { return Type; }
	FName GetID() const { return ID; }

	UFUNCTION()
	void OnSlotClicked();
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnSlotClicked OnSlotClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnSlotToggled OnSlotToggledDel;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Grade;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Selected;

	UPROPERTY(EditAnywhere, Category = "LR|UI Slot")
	int32 SlotIndex = 0;

	UPROPERTY(EditAnywhere, Category = "LR|UI|Empty")
	TObjectPtr<class UTexture2D> EmptySlotTexture;

	bool bIsSelected = false;
	FName ID;
	ECollectionType Type = ECollectionType::NONE;
};
