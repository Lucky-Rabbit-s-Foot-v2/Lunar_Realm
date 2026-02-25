// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LRAnimInstance.generated.h"

/**
 * 
 */

//=============================================================================
// (260224) BJM 제작. ABP 애님 인스턴스 생성.
//=============================================================================

class ALRCharacter;
class UCharacterMovementComponent;

UCLASS()
class LUNAR_REALM_API ULRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	float Speed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsMoving;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<ALRCharacter> OwnerCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
