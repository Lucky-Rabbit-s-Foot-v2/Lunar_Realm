// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRCollection.generated.h"

//============================================================================
/**
 * 도감 위젯 스위처
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRCollection : public ULRChildWidget
{
	GENERATED_BODY()
	
public:	
	virtual void InitializeUI() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RegisterSubWidgets() override;

	UFUNCTION()
	void OnBtnCharacterClicked();

	UFUNCTION()
	void OnBtnEquipClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> Switcher_Collection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterCollection> CharacterCollection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREquipCollection> EquipCollection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Character;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Equip;

};
