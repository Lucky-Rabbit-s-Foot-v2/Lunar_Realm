// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "Data/LREnumType.h"
#include "Data/LRDataStructs.h"
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

	virtual void BindToController(class ALRControllerBase* Controller) override;

	UFUNCTION()
	void SetIDByType(const FSelectedInfo& InInfo);

private:
	void SwitchWidgetByType(ECollectionType InType);

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
};
