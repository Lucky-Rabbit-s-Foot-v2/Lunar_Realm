// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPersistentWidget.h"
#include "LROutGamePersistentWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULROutGamePersistentWidget : public ULRPersistentWidget
{
	GENERATED_BODY()

public:
	virtual void RegisterSubWidgets() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCurrencyWidget> Currency = nullptr;
};
