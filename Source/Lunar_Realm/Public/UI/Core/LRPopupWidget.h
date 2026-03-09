// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPopupWidget.generated.h"

//============================================================================
/**
 * 유저가 직접 열고 닫는 창
 * - 캐릭터 정보, 인벤토리 덱 구성 등
 */
 //============================================================================
 // (260219) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPopupWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	ULRPopupWidget(const FObjectInitializer& ObjectInitializer);

};
