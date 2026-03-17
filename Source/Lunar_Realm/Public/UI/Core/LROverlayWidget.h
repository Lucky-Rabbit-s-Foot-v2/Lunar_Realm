// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LROverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULROverlayWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	ULROverlayWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void OpenUI() override;
};
