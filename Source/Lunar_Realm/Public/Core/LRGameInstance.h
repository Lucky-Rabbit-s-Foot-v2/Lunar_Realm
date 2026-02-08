// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LRGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "LR|Stage")
	void SetCurrentStageID(int32 InStageID) { CurrentStageID = InStageID; }

	UFUNCTION(BlueprintPure, Category = "LR|Stage")
	int32 GetCurrentStageID() const { return CurrentStageID; }

private:
	// NOTE(260208, Codex): Stage 관리 주체가 확정 전이므로 GameInstance 보관을 기본 가정으로 적용.
	UPROPERTY(EditAnywhere, Category = "LR|Stage")
	int32 CurrentStageID = 1;
};
