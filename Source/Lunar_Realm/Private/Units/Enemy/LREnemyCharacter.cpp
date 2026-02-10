// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyCharacter.h"

#include "Abilities/GameplayAbility.h"
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

void ALREnemyCharacter::InitializeByEnemyID(FName EnemyID)	// FIX(KWB) : FName으로 수정 필요한 부분
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

void ALREnemyCharacter::InitializeAttributes(FName EnemyID)
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

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);	// FIX(KWB) : FName으로 수정 필요한 부분

	AttributeSet->InitHealth(static_cast<float>(EnemyData.MaxHealth));
	AttributeSet->InitAttack(static_cast<float>(EnemyData.Attack));
	AttributeSet->InitSpeed(static_cast<float>(EnemyData.Speed));

	ClearGrantedEnemyAbilities();
	GrantEnemyAbilities();
}

void ALREnemyCharacter::GrantEnemyAbilities()	// FIX(KWB) : FName으로 수정 필요한 부분
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		return;
	}

	TArray<FName> SkillIDs = DataSys->GetEnemySkillIDs(CurrentEnemyID);	// FIX(KWB) : FName으로 수정 필요한 부분

	for (FName SkillID : SkillIDs)
	{
		const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);	// FIX(KWB) : FName으로 수정 필요한 부분

		for (const TSoftClassPtr<UGameplayAbility>& SoftAbilityClass : SkillData.GrantedAbilities)
		{
			if (SoftAbilityClass.IsNull())
			{
				LR_WARN(TEXT("Invalid Soft Class Reference in Enemy Skill %s"), *SkillID.ToString());
				continue;
			}

			TSubclassOf<UGameplayAbility> AbilityClass = SoftAbilityClass.LoadSynchronous();

			if (!AbilityClass)
			{
				LR_ERROR(TEXT("Failed to load Ability Class for Enemy Skill %s"), *SkillID.ToString());
				continue;
			}

			FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);

			if (Handle.IsValid())
			{
				GrantedAbilityHandles.Add(Handle);
			}
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
