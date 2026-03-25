// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPartySlotWidget.generated.h"

/**
 * 
 */


UCLASS()
class LUNAR_REALM_API ULRPartySlotWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RegisterSubWidgets() override;

	void SetSlotIndex(int32 InIndex);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartyCharacterSlot> CharacterSlot;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartyEquipmentSlot> EquipmentSlot;

	UPROPERTY(EditAnywhere, Category = "LR|UI Party")
	int32 SlotIndex = 0;
};
