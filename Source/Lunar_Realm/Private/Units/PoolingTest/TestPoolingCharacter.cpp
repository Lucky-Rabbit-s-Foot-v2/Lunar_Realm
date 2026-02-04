// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/PoolingTest/TestPoolingCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void ATestPoolingCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ATestPoolingCharacter::OnPoolActivate_Implementation()
{
	FVector DesiredLocation(500, 0, 500);
	FVector BestLocation = DesiredLocation;
	FRotator BestRotation = GetActorRotation();
	if (GetWorld()->FindTeleportSpot(this, BestLocation, BestRotation))
	{
		SetActorLocation(BestLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else
	{
		SetActorLocation(DesiredLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	SetActorTickEnabled(true);
	
	GetCapsuleComponent()->SetSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

void ATestPoolingCharacter::OnPoolDeactivate_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetMesh()->SetSimulatePhysics(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	SetActorTickEnabled(false);

	FVector NewLocation(2000, 0, 0);
	SetActorLocation(NewLocation);

}
