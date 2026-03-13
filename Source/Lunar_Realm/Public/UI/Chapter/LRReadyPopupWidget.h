// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRReadyPopupWidget.generated.h"

//============================================================================
/**
 * 스테이지 선택 버튼 누를 시 표시할 스테이지 준비 UI 위젯
 */
 //============================================================================
 // (260227) PJB 제작. 제반 사항 구현
 //============================================================================


UCLASS()
class LUNAR_REALM_API ULRReadyPopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshUI() override;
	
	virtual void RegisterSubWidgets() override;

	virtual void SetStageID(FName InStageID);
	
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
	TObjectPtr<class UButton> Btn_EmptyPoint;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Entrance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Close;

private:
	FName StageID;
};
