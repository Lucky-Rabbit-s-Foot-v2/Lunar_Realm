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
	virtual void NativeOnInitialized() override;

	virtual void RefreshUI() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRReadySlot> Main;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRReadySlot> Member1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRReadySlot> Member2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRReadySlot> Member3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRReadySlot> Member4;

	UPROPERTY(EditAnywhere, Category = "LR|UI|Empty")
	TObjectPtr<class UTexture2D> EmptySlotTexture;

	UPROPERTY()
	TArray<class ULRReadySlot*> MemberSlots;
};
