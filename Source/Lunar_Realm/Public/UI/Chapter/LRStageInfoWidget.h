// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRStageInfoWidget.generated.h"

//============================================================================
/**
 * 스테이지 정보 표시
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRStageInfoWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	void RegisterSubWidgets() override;
	
	void SetStageDataByID(FName InStageID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_StageName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnemyInfo> Enemy1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnemyInfo> Enemy2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREnemyInfo> Enemy3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRRewardInfoWidget> Reward1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRRewardInfoWidget> Reward2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRRewardInfoWidget> Reward3;

private:
	FName CurrentStageID;

	TArray<class ULREnemyInfo*> EnemyInfos;
	TArray<class ULRRewardInfoWidget*> RewardInfos;
};
