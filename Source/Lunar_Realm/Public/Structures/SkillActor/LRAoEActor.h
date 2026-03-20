// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LRAoEActor.generated.h"




class UAbilitySystemComponent;
class UGameplayEffect;
class UNiagaraComponent;
class USphereComponent;
//=============================================================================
// (260320) BJM 제작. Nurse전용기 장판 엑터 .
//=============================================================================
UCLASS()
class LUNAR_REALM_API ALRAoEActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALRAoEActor();

	void InitAoE(UAbilitySystemComponent* InInstigatorASC, TSubclassOf<UGameplayEffect> InDamageGE, FGameplayTag InHostileTag, FName InResourceID, float InRadius, float InDuration);

protected:
	UFUNCTION()
	void ApplyPeriodicDamage();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|AoE")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|AoE")
	UNiagaraComponent* AoENiagaraComp;

private:
	TWeakObjectPtr<UAbilitySystemComponent> InstigatorASC;
	FGameplayTag HostileTag;
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	FName CachedResourceID;

	float TickRate;
	FTimerHandle DamageTimerHandle;

};
