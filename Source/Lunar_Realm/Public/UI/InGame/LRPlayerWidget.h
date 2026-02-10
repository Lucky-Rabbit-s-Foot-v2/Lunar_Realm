// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRPlayerWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260204) BJM 제작. 인게임 관련 Widget 생성.
// (260209_BJM) OpenUI Visible -> SelfHitTestInvisible 로 변경
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRPlayerWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	// 컨트롤러 세팅이 끝나면 실행될 이벤트 (BP에서 UI 갱신)
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

public:
	virtual void OpenUI() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void InitializeGAS(UAbilitySystemComponent* ASC);

};
