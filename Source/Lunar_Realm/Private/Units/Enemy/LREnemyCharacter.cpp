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

#include "Core/Stage/LRStageGameState.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"

#include "Units/LRAIController.h"
#include "Units/Enemy/LREnemyAIController.h"

ALREnemyCharacter::ALREnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<ULREnemyAttributeSet>(TEXT("AttributeSet"));
}

void ALREnemyCharacter::OnDie()
{
	// 게임 스테이트에 에테르 추가
	if (ALRStageGameState* GameState = GetWorld()->GetGameState<ALRStageGameState>())
	{
		float DropAether = GetDropAetherAmount();
		GameState->AddAether(DropAether);
	}

	 // 1. AI 비활성화
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (AIController->BrainComponent)
		{
			AIController->BrainComponent->StopLogic(TEXT("Dead"));
		}
	}

	// 2. 충돌 비활성화 (추가 공격 방지)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3. 입력 및 움직임 비활성화
	DisableInput(nullptr);
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// 4. 스켈레탈 메시 해제
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		// 메시를 null로 설정하여 완전히 해제
		MeshComp->SetSkeletalMesh(nullptr);

		// 또는 시각적으로만 숨기려면:
		// MeshComp->SetVisibility(false);
		// MeshComp->SetHiddenInGame(true);

		// 물리 비활성화
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 물리 안보이기
		MeshComp->SetVisibility(false);
	}

	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (!PoolSys)
	{
		LR_ERROR(TEXT("PoolingSubsystem not found while returning enemy to pool"));
		return;
	}

	PoolSys->ReturnToPool(this);
	LR_DEBUG(TEXT("%s : 에너미 사망 -> 풀로 돌아감"), *GetName());
}

void ALREnemyCharacter::InitializeByEnemyID(FName EnemyID)
{
	// TEMP : #55 안움직이는 버그 수정 때 필요
	//LR_INFO(TEXT("[%s] InitializeByEnemyID - Controller: %s"),
	//	*GetName(),
	//	GetController() ? *GetController()->GetName() : TEXT("NULL"));

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

	// BehaviorTree 설정 및 실행
	if (EnemyData.BehaviorTree)
	{
		UBehaviorTree* BT = EnemyData.BehaviorTree.Get();

		if (BT)
		{
			if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
			{
				AICtrl->InitializeBehaviorTree(BT);
			}
			else
			{
				LR_WARN(TEXT("[%s] : No valid LRAIController"), *EnemyID.ToString());
			}
		}
		else
		{
			LR_ERROR(TEXT("[%s] : Failed to get BT"), *EnemyID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] : No valid BT in DataTable"), *EnemyID.ToString());
	}
}

void ALREnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AbilitySystemComponent->AddLooseGameplayTag(UnitTag);
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

			MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

			// 높이 보정: 메시 발바닥을 캡슐 바닥에 맞춘다
			FBoxSphereBounds ImportedBounds = LoadedMesh->GetImportedBounds();
			float MeshBottomZ = ImportedBounds.Origin.Z - ImportedBounds.BoxExtent.Z;
			float HalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

			float RelativeZ = -HalfHeight - MeshBottomZ;
			MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, RelativeZ));
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
			// LR_INFO(TEXT("Enemy [%s] AnimBP set to [%s]"), *CurrentEnemyID.ToString(), *AnimClass->GetName());	// TODO: AnimBP 넣고 테스트
		}
		else
		{
			// LR_WARN(TEXT("Failed to load AnimBlueprintClass for [%s]"), *CurrentEnemyID.ToString());
		}
	}

	const float FinalScale = (EnemyData.Scale > KINDA_SMALL_NUMBER) ? EnemyData.Scale : 1.0f;
	SetActorScale3D(FVector(FinalScale));
}

float ALREnemyCharacter::GetDropAetherAmount() const
{
	if (CurrentEnemyID == NAME_None)
	{
		LR_WARN(TEXT("Not Valid CurrentEnemyID"));
		return 0.0f;
	}

	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;

	if (!DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found in GetDropAetherAmount"));
		return 0.0f;
	}

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(CurrentEnemyID);

	return EnemyData.DropAether;
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
			else
			{
				LR_ERROR(TEXT("Failed to grant ability: %s"), *AbilityClass->GetName());
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
	if (!GetController())
	{
		UPoolingSubsystem* PoolingSystem = GetWorld()->GetSubsystem<UPoolingSubsystem>();
		if (PoolingSystem)
		{
			AController* PooledController = Cast<AController>(
				PoolingSystem->SpawnPooledActor(
					ALREnemyAIController::StaticClass(),
					FTransform::Identity
				)
			);

			if (PooledController)
			{
				PooledController->Possess(this);
			}
		}
	}

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
		MoveComp->SetComponentTickEnabled(true);
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ALREnemyCharacter::OnPoolDeactivate_Implementation()
{
	if (AController* Ctrl = GetController())
	{
		UPoolingSubsystem* PoolingSystem = GetWorld()->GetSubsystem<UPoolingSubsystem>();
		if (PoolingSystem)
		{
			PoolingSystem->ReturnToPool(Ctrl);
		}

		Ctrl->UnPossess();
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
		MoveComp->SetComponentTickEnabled(false);
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
