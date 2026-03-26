// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"
#include "LREnhanceButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULREnhanceButtonWidget : public ULRButtonWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativePreConstruct() override;

	virtual void RefreshUI() override;

	virtual void OnButtonClicked() override;

	void SetCharacterID(const FName& InID);
	void SetEquipID(const FName& InID);

	void CalculateExp(int32 CurrentLevel, int32 CurrentExp, int32 MaxLevel);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Cost = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Enhance")
	int32 EnhanceCount = 0;

	FName ID = NAME_None;

	const int32 CostPerExp = 10;
	const int32 MaxLevel = 100;
	int32 EnhanceExp = 0;
	int32 EnhanceCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Enhance")
	int32 CurrentTypeIndex = 0;
};
