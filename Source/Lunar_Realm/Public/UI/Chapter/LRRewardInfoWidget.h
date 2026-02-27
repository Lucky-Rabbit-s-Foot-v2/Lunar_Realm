// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "LRRewardInfoWidget.generated.h"

//============================================================================
/**
 * 스테이지 정보에서 보상 정보 표시 위젯
 */
 //============================================================================
 // (260227) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRRewardInfoWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	virtual void SetRewardAmount(const int32 InAmount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Star;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Reward;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Amount;

private:
	int32 RewardAmount = 0;
};
