// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPopupWidget.generated.h"

//============================================================================
/**
 * UI Layer 에 팝업으로 표시될 위젯의 기본 클래스
 */
 //============================================================================
 // (260219) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPopupWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;

	virtual void BindToPlayerController(class ALRControllerBase* PC);
};
