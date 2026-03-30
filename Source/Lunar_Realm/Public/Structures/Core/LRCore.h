// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "TimerManager.h"
#include "LRCore.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class ULRCoreAttributeSet;
class ULRAttributeSet;

/**
 * LRCore 구성 요소
 * 각 진영 코어의 베이스 클래스
 * - 콜리전
 * - 메쉬
 * - 체력
 */

//============================================================================
// (260204) KWB 제작. 현재 기본틀만 구성
// (260217) BJM CoreAttribute 적용
//============================================================================

UCLASS()
class LUNAR_REALM_API ALRCore : public AActor, public IGameplayTagAssetInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALRCore();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
	{
		TagContainer = OwnedTags;
	}

	FORCEINLINE UBoxComponent* GetHitCollision() const { return HitCollision; }
	FORCEINLINE UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

	virtual void OnCoreDestroyed();

protected:
	virtual void BeginPlay() override;

	virtual void OnHealthChanged(const FOnAttributeChangeData& InData);


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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<ULRAttributeSet> AttributeSet;

	bool bIsDestroyed;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stage")
	FName StageTag;

	// (260324) KHS 추가. StatusBarSlot에서 아이콘 표시에 사용. BP에서 직접 할당.
	UPROPERTY(EditDefaultsOnly, Category = "LR|Visual")
	TObjectPtr<UTexture2D> CoreIcon;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Destruction")
	class UStaticMesh* DestroyedMeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Destruction")
	class UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Destruction")
	class USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Destruction")
	TSubclassOf<class UCameraShakeBase> DestructionCameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<class UNiagaraComponent> DustNiagaraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<class UNiagaraComponent> DustNiagaraComp2;

	FTimerHandle CollapseUpdateTimerHandle;

	FVector InitialMeshLocation;

	float CollapseElapsedTime;

protected:

	void UpdateCollapseSequence();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|CoreStat")
	float MaxCoreHealth = 1000.0f;
};
