// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRCharacterCard.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCharacterCard : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;
	
	void SetCharacterID(const FName& InID);
	void UpdateCharacterData();

	void SetEmptyCase(bool bIsEmpty);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> Border;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Portrait;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Grade;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|CharacterCard")
	TObjectPtr<class UTexture2D> EmptyBG;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|CharacterCard")
	TObjectPtr<class UTexture2D> DefaultBG;

private:
	FName ID = NAME_None;
};
