// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRStageReadyWidget.generated.h"

//============================================================================
/**
 * 스테이지 선택 버튼 누를 시 표시할 스테이지 준비 UI 위젯
 */
 //============================================================================
 // (260227) PJB 제작. 제반 사항 구현
 //============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageOpenClicked, FName, StageID);


UCLASS()
class LUNAR_REALM_API ULRStageReadyWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshUI() override;
	
	virtual void SetStageID(FName InStageID);

	UPROPERTY(BlueprintAssignable, Category = "LR|Event")
	FOnStageOpenClicked OnStageOpenClickedDel;

	UFUNCTION()
	void OnEntranceButtonClicked();

	UFUNCTION()
	void OnCloseButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartyLineupWidget> PartyLineup;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageInfoWidget> StageInfo;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Entrance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Close;

private:
	FName StageID;
};
