// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRBossAlertPopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRBossAlertPopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void OpenUI() override;
	
	UFUNCTION(BlueprintCallable)
	void OnPlayAlertAnimation();
	
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

protected:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AlertAnim;

};
