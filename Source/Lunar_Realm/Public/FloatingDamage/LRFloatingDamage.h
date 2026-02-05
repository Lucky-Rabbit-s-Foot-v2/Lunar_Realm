// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LRPoolableInterface.h"
#include "LRFloatingDamage.generated.h"

/**
 * 데미지(Projectile) 베이스 클래스
 * - 오브젝트 풀링 처리
 */
//============================================================================
// (260203) PJB 제작. 제반 사항 구현.
//============================================================================

UCLASS()
class LUNAR_REALM_API ALRFloatingDamage : public AActor, public ILRPoolableInterface
{
	GENERATED_BODY()
	
public:	
	ALRFloatingDamage();

protected:
	virtual void BeginPlay() override;

	virtual void OnPoolActivate_Implementation();
	virtual void OnPoolDeactivate_Implementation();

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	TSubclassOf<ALRFloatingDamage> GetBaseClass() const { return TSubclassOf<ALRFloatingDamage>(GetClass()); }
};
