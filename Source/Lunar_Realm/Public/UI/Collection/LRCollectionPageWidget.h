// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRCollectionPageWidget.generated.h"


//============================================================================
/**
 * 도감 페이지 위젯
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRCollectionPageWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
protected:
	virtual void RegisterSubWidgets() override;
	
	virtual void InitializeUI() override;

	virtual void BindToController(class ALRControllerBase* Controller) override;

	UFUNCTION()
	void SetIDByType(ESelectedType InType, FName InID);

private:
	void SwitchWidgetByType(ESelectedType InType);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> Switcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREquipmentInfo> EquipmentInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterInfoWidget> CharacterInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCollection> Collection;

private:
	FName ID = NAME_None;
};
