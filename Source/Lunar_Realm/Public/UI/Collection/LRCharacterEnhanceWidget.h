// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
#include "LRCharacterEnhanceWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCharacterEnhanceWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RegisterSubWidgets() override;

	UFUNCTION()
	void RefreshUICall();

	void SetCharacterID(const FName& InID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterCard> CharacterCard = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterStatusWidget> CharacterStatus = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnhanceButtonWidget> Enhance1 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnhanceButtonWidget> Enhance5 = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnhanceButtonWidget> Enhance10 = nullptr;

	FName ID = NAME_None;
	FCharacterInstance CharacterData;
};
