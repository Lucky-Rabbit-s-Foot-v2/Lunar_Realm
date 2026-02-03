// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "LRPlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRPlayerWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	// 컨트롤러 정보를 세팅
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

protected:
	// 컨트롤러 저장 변수
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	// 컨트롤러 세팅이 끝나면 실행될 이벤트 (BP에서 UI 갱신)
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
