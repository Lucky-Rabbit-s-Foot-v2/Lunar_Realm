// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyCharacter.h"

#include "Engine/GameInstance.h"
#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"

ALREnemyCharacter::ALREnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<ULREnemyAttributeSet>(TEXT("AttributeSet"));
}

void ALREnemyCharacter::OnDie()
{
	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (!PoolSys)
	{
		LR_ERROR(TEXT("PoolingSubsystem not found while returning enemy to pool"));
		return;
	}

	PoolSys->ReturnToPool(this);
}

void ALREnemyCharacter::InitializeByEnemyID(int32 EnemyID)
{
	CurrentEnemyID = EnemyID;
	InitializeAttributes(EnemyID);
}

void ALREnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ALREnemyCharacter::InitializeAttributes(int32 EnemyID)
{
	if (!AttributeSet || !AbilitySystemComponent)
	{
		LR_ERROR(TEXT("AttributeSet or ASC is null"));
		return;
	}

	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;

	if (!DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found"));
		return;
	}

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);

	AttributeSet->InitHealth(static_cast<float>(EnemyData.Health));
	AttributeSet->InitAttack(static_cast<float>(EnemyData.Attack));
	AttributeSet->InitSpeed(static_cast<float>(EnemyData.Speed));

	ClearGrantedEnemyAbilities();
	GrantEnemyAbilities(EnemyData.GrantedAbilities);
}

void ALREnemyCharacter::GrantEnemyAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : InAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}

		const FGameplayAbilitySpec Spec(AbilityClass, 1);
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		if (Handle.IsValid())
		{
			GrantedAbilityHandles.Add(Handle);
		}
	}
}

void ALREnemyCharacter::ClearGrantedEnemyAbilities()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
	{
		if (Handle.IsValid())
		{
			AbilitySystemComponent->ClearAbility(Handle);
		}
	}

	GrantedAbilityHandles.Reset();
}
