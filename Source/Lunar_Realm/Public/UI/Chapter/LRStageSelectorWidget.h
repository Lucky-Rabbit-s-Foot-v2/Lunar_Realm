// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LRPopupWidget.h"
#include "LRStageSelectorWidget.generated.h"

//============================================================================
/**
 * 스테이지 선택 UI 위젯
 * - 여러 스테이지 표시하고 선택 가능
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRStageSelectorWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OnBackButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UStageWidget> Stage1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UStageWidget> Stage2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UStageWidget> Stage3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UStageWidget> Stage4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UStageWidget> Stage5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Back;
};
