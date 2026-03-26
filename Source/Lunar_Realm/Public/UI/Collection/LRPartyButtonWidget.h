// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "Data/LREnumType.h"
#include "Data/LRDataStructs.h"

#include "LRPartyButtonWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartyButtonClicked, const FSelectedInfo&, InInfo);

UCLASS()
class LUNAR_REALM_API ULRPartyButtonWidget : public ULRButtonWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindToController(class ALRControllerBase* Controller) override;

	virtual void OnButtonClicked() override;
	
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnPartyButtonClicked OnPartyButtonClickedDel;

protected:
	EPartyTaskType Type = EPartyTaskType::NONE;
};
