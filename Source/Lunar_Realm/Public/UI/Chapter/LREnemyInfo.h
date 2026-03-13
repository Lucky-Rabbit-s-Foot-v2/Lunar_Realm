// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LREnemyInfo.generated.h"


//============================================================================
/**
 * 스테이지 준비할 때 표시할 몬스터 초상화
 */
 //============================================================================
 // (260227) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULREnemyInfo : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	void SetEnemyID(FName InEnemyID);

protected:
	UPROPERTY(VisibleAnywhere)
	FName EnemyID;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Portrait;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

};
