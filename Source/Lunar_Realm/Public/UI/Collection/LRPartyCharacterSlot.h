// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRSlotWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRPartyCharacterSlot.generated.h"

/**
 * 
 */

UCLASS()
class LUNAR_REALM_API ULRPartyCharacterSlot : public ULRSlotWidget
{
	GENERATED_BODY()

public:	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void RefreshOnSaveGameChanged();

	virtual void SetSlotIndex(int32 InIndex) override;
	virtual void SetIDAuto() override;
	virtual void SetID(FName InID) override;
	virtual void SetGradeImage() override;
	virtual void SetIconImage() override;
};
