// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LREnhancePageWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULREnhancePageWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
public:
	virtual void RegisterSubWidgets() override;

	virtual void InitializeUI() override;

	void SetCharacterID(FName InID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterEnhanceWidget> CharacterEnhance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCollection> Collection;

	FName CharacterID = FName();
};
