// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRReadySlot.generated.h"

/**
 * 
 */

class UTexture2D;

UCLASS()
class LUNAR_REALM_API ULRReadySlot : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	void SetData(UTexture2D* InPortrait, UTexture2D* InGrade, UTexture2D* InEquip);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Portrait;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Grade;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Equip;

};
