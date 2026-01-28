// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LRPoolableInterface.h"
#include "LRProjectile.generated.h"

/**
 * 투사체(Projectile) 베이스 클래스
 * - 오브젝트 풀링 처리
 */
 //============================================================================
 // (260128) PJB 제작. 제반 사항 구현.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALRProjectile : public AActor, public ILRPoolableInterface
{
	GENERATED_BODY()
	
public:	
	ALRProjectile();

protected:
	virtual void BeginPlay() override;

	virtual void OnPoolActivate_Implementation();
	virtual void OnPoolDeactivate_Implementation();

public:	
	virtual void Tick(float DeltaTime) override;

	/**
	* 오브젝트 풀링 헬퍼 함수
	* - 자손 클래스의 베이스 클래스 타입 반환
	*/
	UFUNCTION(BlueprintPure)
	TSubclassOf<ALRProjectile> GetBaseClass() const { return TSubclassOf<ALRProjectile>(GetClass()); }

	// TODO: Projectile Movement Component 추가
};
