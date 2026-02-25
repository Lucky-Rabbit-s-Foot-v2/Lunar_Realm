// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRSystemWidget.generated.h"

//============================================================================
/**
 * 최상단 강제 노출 시스템 위젯
 * - 디버그 정보, 시스템 메시지 등
 */
 //============================================================================
 // (260224) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRSystemWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRSystemWidget(const FObjectInitializer& ObjectInitializer);

};
