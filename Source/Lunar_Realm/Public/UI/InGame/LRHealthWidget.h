// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LRPopupWidget.h"
#include "GameplayEffectTypes.h"
#include "LRHealthWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260209) BJM 제작. Health UI 위젯 클래스 생성.
//=============================================================================

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class LUNAR_REALM_API ULRHealthWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void BindToASC(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void UpdateHealth(float InCurrentHealth, float InMaxHealth);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PBar_Health;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	float CurrentHealth = 0.0f;
	float CurrentMaxHealth = 1.0f;
};
