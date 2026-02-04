// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "LRCore.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * LRCore 구성 요소
 * 각 진영 코어의 베이스 클래스
 * - 콜리전
 * - 메쉬
 * - 체력
 */
 //============================================================================
 // (260204) KWB 제작. 현재 기본틀만 구성
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALRCore : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALRCore();

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = OwnedTags;
	}

	FORCEINLINE UBoxComponent* GetHitCollision() const { return HitCollision; }
	FORCEINLINE UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

	// TEMP
	int32 GetHealth() { return Health; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHitCollisionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> HitCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> VisualMesh = nullptr;

public:
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTagContainer OwnedTags;

	// TEMP
	UPROPERTY(EditAnywhere, Category = "TEMP")
	int32 Health = 100;
};
