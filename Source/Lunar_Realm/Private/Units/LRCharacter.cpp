// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRCharacter.h"

ALRCharacter::ALRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ALRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

