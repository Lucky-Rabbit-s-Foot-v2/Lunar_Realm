// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRCharacterEnhanceWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCharacterEnhanceWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Main = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Grade = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterStatusWidget> CharacterStatus = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnhanceButtonWidget> Enhance1 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnhanceButtonWidget> Enhance5 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnhanceButtonWidget> Enhance10 = nullptr;

	FName CharacterID = FName();
};
