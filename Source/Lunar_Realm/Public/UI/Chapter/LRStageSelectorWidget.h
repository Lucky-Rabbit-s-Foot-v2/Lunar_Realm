// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
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
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RegisterSubWidgets() override;

	UFUNCTION(BlueprintCallable)
	void SetStageData(const TArray<FName>& StageIDs);

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OnBackButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Back;

	UPROPERTY(VisibleAnywhere, Category = "LR|UI")
	TArray<TObjectPtr<ULRStageWidget>> StageWidgets;
};
