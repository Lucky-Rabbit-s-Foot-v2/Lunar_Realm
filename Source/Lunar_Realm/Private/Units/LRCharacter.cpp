// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRCharacter.h"

#include "Components/CapsuleComponent.h"

ALRCharacter::ALRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);  

	// (260227) KHS UnitTag를 ASC에도 등록
	if (UnitTag.IsValid())
	{
		UAbilitySystemComponent* ASC = 
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this);
		if (ASC)
		{
			ASC->AddLooseGameplayTag(UnitTag);
		}
	}
}

void ALRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRCharacter::OnPoolActivate_Implementation()
{
	// TODO: 인터페이스 참고
}

void ALRCharacter::OnPoolDeactivate_Implementation()
{
	// TODO: 인터페이스 참고
}

void ALRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

