// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRChapterWidget.generated.h"


//============================================================================
/**
 * 챕터 UI 위젯
 * - 챕터 정보 들고 있어야 함.
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRChapterWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void OpenUI() override;
	virtual void CloseUI() override;
	virtual void RefreshUI() override;

};
