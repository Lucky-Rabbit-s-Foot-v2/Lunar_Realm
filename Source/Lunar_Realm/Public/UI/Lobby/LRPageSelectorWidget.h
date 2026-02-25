// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRPageSelectorWidget.generated.h"


//============================================================================
/**
 * 로비 화면 내 페이지 선택 위젯
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현.
 //============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollectionButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPartyButtonClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGachaButtonClicked);

UCLASS()
class LUNAR_REALM_API ULRPageSelectorWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI")
	FOnStageButtonClicked OnStageButtonClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI")
	FOnCollectionButtonClicked OnCollectionButtonClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI")
	FOnPartyButtonClicked OnPartyButtonClickedDel;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI")
	FOnGachaButtonClicked OnGachaButtonClickedDel;

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
	TObjectPtr<class ULRBaseWidget> Profile;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Stage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Party;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Collection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Gacha;
};
