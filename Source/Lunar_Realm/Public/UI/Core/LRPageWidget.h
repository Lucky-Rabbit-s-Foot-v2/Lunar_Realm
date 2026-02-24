// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPageWidget.generated.h"


//============================================================================
/**
 * 페이지 위젯
 * - 게임의 주요 화면을 구성하는 UI
 * - 전환 시 쌓인 팝업 위젯 전부 제거
 */
 //============================================================================
 // (260224) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPageWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRPageWidget(const FObjectInitializer& ObjectInitializer);

};
