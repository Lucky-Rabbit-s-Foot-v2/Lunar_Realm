// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRPersistentWidget.generated.h"


//============================================================================
/**
 * UI Layer 의 배경이 될 위젯
 */
 //============================================================================
 // (260219) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPersistentWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;
	virtual void OpenUI() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Persistant")
	TSubclassOf<class ULRPopupWidget> DefaultPopupClass;
};
