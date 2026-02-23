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
// =============================================================================

class UBoxComponent;
UCLASS()
class LUNAR_REALM_API ALRPlayerCore : public ALRCore
{
	GENERATED_BODY()

public:
	ALRPlayerCore();

	UFUNCTION(BlueprintCallable, Category = "Summon")
	FVector GetRandomSpawnLocation() const;

protected:
	virtual void OnCoreDestroyed() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<UBoxComponent> SpawnArea;


};
