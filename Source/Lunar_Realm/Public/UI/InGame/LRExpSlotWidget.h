// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRExpSlotWidget.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;
class UProgressBar;

UCLASS()
class LUNAR_REALM_API ULRExpSlotWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	


public:
	void SetSlotInfo(FName InCharID, int32 InOldLevel, float InOldExp, float InGainedExp);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void RefreshUI() override;

	UPROPERTY(meta = (BindWidget))
	UImage* CharIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* GradeImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentExp;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxExp;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ExpBar;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* LevelUpAnim;




private:
	FName CharacterID;
	int32 CurrentLevel;

	float CurrentVisualExp;
	float TargetExp;
	float MaxExpAmount;
	float FillSpeed = 500.0f;
	bool bIsFilling = false;

	void HandleLevelUp();

private:
	float GetRequiredExpForLevel(int32 InLevel) const;

};
