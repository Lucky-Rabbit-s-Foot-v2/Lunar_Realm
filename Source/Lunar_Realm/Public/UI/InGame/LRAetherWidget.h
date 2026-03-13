// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "GameplayEffectTypes.h"
#include "LRAetherWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260209) BJM 제작. Aether UI 위젯 클래스 생성.
//=============================================================================

class UTextBlock;
class UAbilirtySystemComponent;

UCLASS()
class LUNAR_REALM_API ULRAetherWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void BindToASC(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void UpdateAether(float Amount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AetherAmount;

private:
	void OnAetherChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(meta=(BindWidgetAnim), Transient)
	class UWidgetAnimation* Anim_GetAether;

	float TargetAether = 0.0f;
	float DisplayAether = 0.0f;

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
