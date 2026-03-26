// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRPartyCharacterSlot.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSlotChanged, const FSelectedInfo&, InInfo);

UCLASS()
class LUNAR_REALM_API ULRPartyCharacterSlot : public ULRButtonWidget
{
	GENERATED_BODY()

public:
	virtual void OnButtonClicked() override;
	
	virtual void RefreshUI() override;

	virtual void BindToController(class ALRControllerBase* Controller) override;

	void SetSlotIndex(int32 InIndex);
	void SetCharacterID(FName InID);

	UFUNCTION()
	void RefreshUIByController(const FSelectedInfo& InInfo);

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnCharacterSlotChanged OnCharacterSlotChangedDel;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Grade;
	
	UPROPERTY(EditAnywhere, Category = "LR|UI Party")
	int32 SlotIndex = 0;

	UPROPERTY(EditAnywhere, Category = "LR|UI|Empty")
	TObjectPtr<class UTexture2D> EmptySlotTexture;

private:
	FName ID;

};
