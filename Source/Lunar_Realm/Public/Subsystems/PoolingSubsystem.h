// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interfaces/LRPoolableInterface.h"
#include "PoolingSubsystem.generated.h"

/**
 * Object Pooling 을 담당하는 서브시스템
 * 
 * 주요 기능:
 * - 오브젝트 풀링 기반 액터 생성
 * - 풀 반환
 */

//============================================================================
// (260127) PJB 제작. 제반 사항 구현.
//============================================================================


UCLASS()
class LUNAR_REALM_API UPoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	/**
	* @param ClassToSpawn : 생성할 액터 클래스 (ILRPoolableInterface 가 구현되어 있어야 함)
	*/
	UFUNCTION(BlueprintCallable)
	AActor* SpawnPooledActor(TSubclassOf<AActor> ClassToSpawn, const FTransform& SpawnTransform);

	/**
	* Destroy 대신 사용할 것
	*/
	UFUNCTION(BlueprintCallable)
	void ReturnToPool(AActor* ActorToReturn);

	/**
	 * 실시간 생성 오버헤드 방지를 위해 스테이지 시작 시 미리 일정량을 생성해 둠
	 */
	UFUNCTION(BlueprintCallable)
	void InitializePool(TSubclassOf<AActor> ClassToInit, int32 Count);

private:
	TMap<UClass*, TArray<AActor*>> ActorPool;

};
