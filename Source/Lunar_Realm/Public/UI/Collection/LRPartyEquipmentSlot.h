// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRSlotWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRPartyEquipmentSlot.generated.h"

/**
 * 
 */

UCLASS()
class LUNAR_REALM_API ULRPartyEquipmentSlot : public ULRSlotWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void RefreshUI() override;

	UFUNCTION()
	void RefreshUICaller();

	virtual void SetSlotIndex(int32 InIndex) override;
	virtual void SetIDAuto() override;
	virtual void SetID(FName InID) override;
	virtual void SetGradeImage() override;
	virtual void SetIconImage() override;
};
