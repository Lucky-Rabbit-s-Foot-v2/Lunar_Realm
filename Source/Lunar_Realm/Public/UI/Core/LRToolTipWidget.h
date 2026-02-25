// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRToolTipWidget.generated.h"

//============================================================================
/**
 * 다른 UI 위에 일시적으로 보여주는 정보창
 * - 스킬 설명, 드롭 다운 메뉴 등
 */
 //============================================================================
 // (260224) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRToolTipWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	ULRToolTipWidget(const FObjectInitializer& ObjectInitializer);

protected:
};
