// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRStageWidget.generated.h"


//============================================================================
/**
 * 각 스테이지 UI 위젯
 * - 스테이지 정보 들고 있어야 함.
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageOpenClicked, FName, StageID);

UCLASS()
class LUNAR_REALM_API ULRStageWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|Event")
	FOnStageOpenClicked OnStageOpenClickedDel;

private:
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OnOpenButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Open;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Base;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Chapter")
	FName StageID;
};
