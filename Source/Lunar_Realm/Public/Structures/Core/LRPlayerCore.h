// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures/Core/LRCore.h"
#include "LRPlayerCore.generated.h"

/**
 * 
 */

//=============================================================================
// (260205) BJM 제작. Player Core 클래스 추가.
// (260217) BJM 패배조건 추가
// (260223) BJM 승패로직을 위한 Gamemode 연동
// (260319) KWB 보스 공격 -> 코어 파괴 로직을 위해 "OnCoreDestroyed()" 접근 제한자 수정
// =============================================================================

class UBoxComponent;
UCLASS()
class LUNAR_REALM_API ALRPlayerCore : public ALRCore
{
	GENERATED_BODY()

public:
	ALRPlayerCore();

	virtual void OnCoreDestroyed() override;

	UFUNCTION(BlueprintCallable, Category = "Summon")
	FVector GetRandomSpawnLocation() const;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<UBoxComponent> SpawnArea;
};
