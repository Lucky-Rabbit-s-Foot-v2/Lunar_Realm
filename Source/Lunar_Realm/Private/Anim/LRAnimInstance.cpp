// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/LRAnimInstance.h"
#include "Units/LRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void ULRAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<ALRCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void ULRAnimInstance::NativeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeUpdateAnimation(InDeltaSeconds);

	if (OwnerCharacter && MovementComponent)
	{
		Speed = OwnerCharacter->GetVelocity().Size2D();

		bIsMoving = (Speed > 3.0f) && (MovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.0f);
	}
}
