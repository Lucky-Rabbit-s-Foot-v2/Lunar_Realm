// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LRPopupWidget.h"
#include "LRIntroWidget.generated.h"

// =============================================================================
/**
 * 인트로 위젯
 * - 인트로 애니메이션 재생 담당
 */
 //=============================================================================
 // (260206) PJB 제작. 인트로 위젯 기반 구성.
 // (260212) PJB 수정. UI Manager 연동.
 // =============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIntroAnimFinished);

UCLASS()
class LUNAR_REALM_API ULRIntroWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void BindToController();
	virtual void NativeDestruct() override;

	virtual void OpenUI() override;
	virtual void RefreshUI() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI")
	FOnIntroAnimFinished OnIntroAnimFinishedDel;

private:
	UFUNCTION(BlueprintCallable, Category = "LR|Level Streaming")
	void PlayIntroAnimation();

	UFUNCTION()
	void OnFinishedIntroAnim();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_3;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> FadeAnim;

};
