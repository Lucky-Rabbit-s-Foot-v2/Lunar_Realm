// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPageSelectorWidget.generated.h"


//============================================================================
/**
 * 로비 화면 내 페이지 선택 위젯
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPageSelectorWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UFUNCTION(BlueprintCallable)
	void OnStageButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnCollectionButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnPartyButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnGachaButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Stage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Party;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Collection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Gacha;

};
