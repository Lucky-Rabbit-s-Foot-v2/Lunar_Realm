// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRBackgroundWidget.generated.h"


//============================================================================
/**
 * 전체 화면 UI
 * - 로딩 화면, 로비 화면 등
 */
 //============================================================================
 // (260224) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRBackgroundWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRBackgroundWidget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Background")
	bool bDisableWorldRenderingWhenOpened = false;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_BG;
};
