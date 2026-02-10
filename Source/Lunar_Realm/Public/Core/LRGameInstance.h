// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LRGameInstance.generated.h"

/**
 * 
 */
 //=============================================================================
 // (260208) KWB Stage 관리 주체 GI로 가정 -> 관련 내용 해당 클래스에 작성 (임시)
 // (260210) KWB 키값 타입 int32 -> FName 으로 변경 반영
 // =============================================================================
UCLASS()
class LUNAR_REALM_API ULRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	void SetCurrentStageID(FName InStageID) { CurrentStageID = InStageID; }

	UFUNCTION(BlueprintPure, Category = "LR|Stage")
	FName GetCurrentStageID() const { return CurrentStageID; }

private:
	UPROPERTY(EditAnywhere, Category = "LR|Stage")
	FName CurrentStageID;
};
