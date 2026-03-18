// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRButtonWidget.h"

#include "Subsystems/Settings/MapSettings.h"
#include "LROpenLevelButton.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULROpenLevelButton : public ULRButtonWidget
{
	GENERATED_BODY()
	
public:
	virtual void OnButtonClicked() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI|Open Level Button")
	ELevelName LevelToOpen = ELevelName::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI|Open Level Button")
	bool bShouldOpenImmediately = false;
};
