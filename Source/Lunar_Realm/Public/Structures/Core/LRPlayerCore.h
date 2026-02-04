// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structures/Core/LRCore.h"
#include "LRPlayerCore.generated.h"


class UBoxComponent;
/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRPlayerCore : public ALRCore
{
	GENERATED_BODY()

public:
	ALRPlayerCore();

	UFUNCTION(BlueprintCallable, Category = "Summon")
	FVector GetRandomSpawnLocation() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<UBoxComponent> SpawnArea;


};
