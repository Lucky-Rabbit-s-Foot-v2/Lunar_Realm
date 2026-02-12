// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyCharacter.h"

#include "Abilities/GameplayAbility.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
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

void ALREnemyCharacter::InitializeByEnemyID(FName EnemyID)
{
	CurrentEnemyID = EnemyID;

	// 데이터 서브시스템에서 전체 에너미 데이터 조회
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found in InitializeByEnemyID"));
		return;
	}

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);

	// 어트리뷰트 초기화 (Health, Attack, Speed, AttackSpeed, AttackRange)
	InitializeAttributes(EnemyID);

	// 비주얼 데이터 적용 (SkeletalMesh, AnimBP, Scale)
	ApplyVisualData(EnemyData);

	// 문제 있을 때만 다시 열어서 체크
	//LR_INFO(TEXT("Enemy [%s] fully initialized - HP:%.0f ATK:%.0f SPD:%.0f Scale:%.2f"),
	//	*EnemyID.ToString(),
	//	EnemyData.MaxHealth,
	//	EnemyData.Attack,
	//	EnemyData.Speed,
	//	EnemyData.Scale);
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

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);

	AttributeSet->InitHealth(static_cast<float>(EnemyData.MaxHealth));
	AttributeSet->InitAttack(static_cast<float>(EnemyData.Attack));
	AttributeSet->InitSpeed(static_cast<float>(EnemyData.Speed));

	// MaxHealth 한계치도 데이터 기반으로 갱신
	// AttributeSet의 MaxHealth 리밋을 실제 데이터에 맞춰 설정해야, PreAttributeChange에서 Clamp될 때 정상 동작
	// => TODO: 추후 고민 후 다시 결정 필요
	AttributeSet->MaxHealth = EnemyData.MaxHealth;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = EnemyData.Speed;
	}

	ClearGrantedEnemyAbilities();
	GrantEnemyAbilities();
}

// TODO: 코드 리팩토링 & 정리 필요
void ALREnemyCharacter::ApplyVisualData(const FEnemyStaticData& EnemyData)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		LR_WARN(TEXT("SkeletalMeshComponent not found on Enemy [%s]"), *CurrentEnemyID.ToString());
		return;
	}

	// TSoftObjectPtr이므로 LoadSynchronous()로 실제 에셋을 로드한다.
	// 이미 로드된 에셋이면 즉시 반환되므로 중복 로드 오버헤드는 없다.
	if (!EnemyData.EnemyMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = EnemyData.EnemyMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshComp->SetSkeletalMesh(LoadedMesh);
			// LR_INFO(TEXT("Enemy [%s] mesh set to [%s]"), *CurrentEnemyID.ToString(), *LoadedMesh->GetName());
		}
		else
		{
			LR_WARN(TEXT("Failed to load EnemyMesh for [%s]"), *CurrentEnemyID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("Enemy [%s] has no EnemyMesh assigned in DataTable"), *CurrentEnemyID.ToString());
	}

	if (!EnemyData.AnimBlueprintClass.IsNull())
	{
		UClass* AnimClass = EnemyData.AnimBlueprintClass.LoadSynchronous();
		if (AnimClass)
		{
			MeshComp->SetAnimInstanceClass(AnimClass);
			LR_INFO(TEXT("Enemy [%s] AnimBP set to [%s]"), *CurrentEnemyID.ToString(), *AnimClass->GetName());	// TODO: AnimBP 넣고 테스트
		}
		else
		{
			LR_WARN(TEXT("Failed to load AnimBlueprintClass for [%s]"), *CurrentEnemyID.ToString());
		}
	}

	const float FinalScale = (EnemyData.Scale > KINDA_SMALL_NUMBER) ? EnemyData.Scale : 1.0f;
	SetActorScale3D(FVector(FinalScale));
}

void ALREnemyCharacter::GrantEnemyAbilities()
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

	TArray<FName> SkillIDs = DataSys->GetEnemySkillIDs(CurrentEnemyID);

	for (FName SkillID : SkillIDs)
	{
		const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

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

void ALREnemyCharacter::OnPoolActivate_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionProfileName(TEXT("Pawn"));
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(true, true);
		MeshComp->SetCollisionProfileName(TEXT("CharacterMesh"));
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->SetActive(true);
	}

	// AI 컨트롤러 재빙의
	// SpawnDefaultController()를 호출하면 AIControllerClass에 설정된 컨트롤러가 빙의됨
	// TODO: 테스트 후 다시 확인
	if (!GetController())
	{
		SpawnDefaultController();
	}
}

void ALREnemyCharacter::OnPoolDeactivate_Implementation()
{
	if (AController* Ctrl = GetController())
	{
		Ctrl->UnPossess();
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	SetActorEnableCollision(false);
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorHiddenInGame(true);
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(false, true);
	}

	SetActorTickEnabled(false);

	SetActorLocation(FVector(0.0f, 0.0f, -10000.0f));

	ClearGrantedEnemyAbilities();

	CurrentEnemyID = NAME_None;
}
