// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRExpPanelWidget.generated.h"


class UVerticalBox;

class UTextBlock;
class ULRExpSlotWidget;

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRExpPanelWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void SetupExpPanel();

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* SlotContainer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoticeText;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<ULRExpSlotWidget> ExpSlotClass;

};
