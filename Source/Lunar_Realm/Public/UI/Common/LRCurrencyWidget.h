// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPersistentWidget.h"
#include "LRCurrencyWidget.generated.h"

// =============================================================================
/**
 * 전체 재화 표시 위젯
 */
 //=============================================================================
 // (260223) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRCurrencyWidget : public ULRPersistentWidget
{
	GENERATED_BODY()

public:
	virtual void RegisterSubWidgets() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCurrencyViewWidget> GoldView = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCurrencyViewWidget> CrescentView = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCurrencyViewWidget> FullMoonView = nullptr;
};
