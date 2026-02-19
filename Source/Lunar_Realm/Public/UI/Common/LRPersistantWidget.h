// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRPersistantWidget.generated.h"


//============================================================================
/**
 * UI Layer 의 배경이 될 위젯
 */
 //============================================================================
 // (260219) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPersistantWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRPersistantWidget()
	{
		UILayer = EUILayer::PERSISTENT;
	}

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	virtual void OpenUI() override;
	virtual void CloseUI() override;
	virtual void RefreshUI() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Persistant")
	TSubclassOf<UBaseWidget> DefaultPopupClass;
};
