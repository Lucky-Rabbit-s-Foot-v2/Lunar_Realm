// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LRCombatComponent.generated.h"

//=============================================================================
// (260205) BJM 제작. 전투 관련 로직 생성(수동, 자동).
//=============================================================================

UENUM(BlueprintType)
enum class EAutoCombatState : uint8
{
	Manual,
	Auto,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNAR_REALM_API ULRCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULRCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAutoMode(bool bEnableAuto);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateWeaponInfo(int32 InWeaponID);

	UFUNCTION(BlueprintPure, Category = "Combat")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsAutoMode() const { return CombatState == EAutoCombatState::Auto; }

protected:

	void FindBestTarget();

	void TryAction(float DeltaTime);

	void MoveToTarget(float DeltaTime);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	EAutoCombatState CombatState = EAutoCombatState::Manual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SearchRadius = 1000.0f;

	float CurrentAttackCooldown = 1.0f;
};
