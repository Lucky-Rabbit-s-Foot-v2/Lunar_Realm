// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPartyLineupWidget.generated.h"

//============================================================================
/**
 * 스테이지 준비할 때 표시할 파티 라인업
 */
 //============================================================================
 // (260227) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPartyLineupWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshUI() override;

	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OnRegroupButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Main;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Member1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Member2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Member3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Member4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equip1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equip2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equip3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Regroup;

};
