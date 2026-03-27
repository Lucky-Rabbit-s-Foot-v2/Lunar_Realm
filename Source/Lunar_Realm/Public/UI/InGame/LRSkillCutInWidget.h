// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRSkillCutInWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRSkillCutInWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	

public:
	void InitCutIn(FName InCharacterID);

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Character;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* SkillCutIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* AnimWind;

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* InAnimation) override;
};
