// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRChildWidget.generated.h"

//============================================================================
/**
 * UI Manager 가 자동으로 관리하지 않을 위젯
 */
 //============================================================================
 // (260224) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRChildWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRChildWidget(const FObjectInitializer& ObjectInitializer);

};
