// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRAIController.h"
#include "GameplayTagContainer.h"
#include "LREnemyAIController.generated.h"


/**
 * LRAIController 구성 요소
 * Enemy 기본 컨트롤러 (보스는 따로 구현 예정)
 * - 태그 기반 타겟 추적
 * - RVO 엔진에서 추가 필요
 */
 //============================================================================
 // (260204) KWB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALREnemyAIController : public ALRAIController
{
	GENERATED_BODY()
	
public:
	ALREnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag TargetBaseTag;

	AActor* FindActorWithGameplayTag(const FGameplayTag& Tag) const;

	void MoveToBase();
};
