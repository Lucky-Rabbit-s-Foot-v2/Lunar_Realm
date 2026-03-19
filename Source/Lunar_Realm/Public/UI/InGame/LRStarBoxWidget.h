// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRStarBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRStarBoxWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	void SetStarMasking(int32 InMasking);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star3;

	UPROPERTY(EditAnywhere, Category = "LR|UI|Star")
	TObjectPtr<class UTexture2D> StarOnTexture = nullptr;

	UPROPERTY(EditAnywhere, Category = "LR|UI|Star")
	TObjectPtr<class UTexture2D> StarOffTexture = nullptr;

private:
	int8 StarMasking = 0; // 1 2 4;

	const int8 Star1Mask = 1;
	const int8 Star2Mask = 2;
	const int8 Star3Mask = 4;
};
