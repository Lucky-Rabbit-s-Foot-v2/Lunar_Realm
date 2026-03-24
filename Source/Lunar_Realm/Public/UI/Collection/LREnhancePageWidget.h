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

	void SetCurrentTypeIndex(int32 InIndex);
	void SetMainID(FName InID);

	void SetCharacterID(const FName& InID);
	void SetEquipID(const FName& InID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterEnhanceWidget> CharacterEnhance;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREquipEnhance> EquipEnhance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCollection> Collection;

	FName ID = NAME_None;

	int32 CurrentTypeIndex = 0; // 0: Character, 1: Equipment
};
