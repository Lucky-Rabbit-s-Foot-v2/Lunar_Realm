// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Member/LRMemberCharacter.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"

ALRMemberCharacter::ALRMemberCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);	//cpu 낭비 줄이기

	MemberAttributeSet = CreateDefaultSubobject<ULRPlayerAttributeSet>(TEXT("AttributeSet"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* ALRMemberCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRMemberCharacter::BeginPlay()
{

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[Member] BeginPlay Called!"));

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (MemberAttributeSet)
		{
			MemberAttributeSet->InitHealth(20.0f);
			MemberAttributeSet->InitMaxHealth(50.0f);
			MemberAttributeSet->InitAttackPower(5.0f);

			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				ULRPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRMemberCharacter::OnHealthChangedNative);
			UE_LOG(LogTemp, Warning, TEXT("[Member] Delegate Bound!"));
		}


		UE_LOG(LogTemp, Log, TEXT("SubCharacter GAS Initialized: %s"), *GetName());
	}
}

void ALRMemberCharacter::OnHealthChangedNative(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.NewValue);
}
