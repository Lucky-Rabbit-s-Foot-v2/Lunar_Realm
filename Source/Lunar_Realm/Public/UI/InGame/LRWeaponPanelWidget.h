// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRWeaponPanelWidget.generated.h"

/**
 * 
 */

 //============================================================================
 // (260227) BJM 제작. 무기 스왑 버튼 패널.
 //============================================================================

class UButton;
class ALRControllerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponButtonClickedSignature);

UCLASS()
class LUNAR_REALM_API ULRWeaponPanelWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void BindToController(ALRControllerBase* InController);

public:
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnWeaponButtonClickedSignature OnWeapon1ClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnWeaponButtonClickedSignature OnWeapon2ClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnWeaponButtonClickedSignature OnWeapon3ClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnWeaponButtonClickedSignature OnUnequipClickedDel;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Weapon1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Weapon2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Weapon3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Unequip;

private:
	UFUNCTION()
	void OnWeapon1Clicked();

	UFUNCTION()
	void OnWeapon2Clicked();

	UFUNCTION()
	void OnWeapon3Clicked();

	UFUNCTION()
	void OnUnequipClicked();
	
};
