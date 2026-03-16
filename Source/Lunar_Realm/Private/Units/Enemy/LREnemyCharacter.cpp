// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyCharacter.h"

#include "Abilities/GameplayAbility.h"
#include "Animation/AnimInstance.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"

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

#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

ALREnemyCharacter::ALREnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<ULREnemyAttributeSet>(TEXT("AttributeSet"));

	//(260316) BJM: 타겟팅 마커(머리 위 화살표) UI 연동을 위한 함수 추가
	SetupTargetMarker();

	//(260316) BJM: 타겟팅 마커(머리 위 화살표) UI 연동을 위한 컴포넌트 추가
	//TargetMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMarkerMesh"));
	//TargetMarkerMesh->SetupAttachment(RootComponent);

	//TargetMarkerMesh->SetCollisionProfileName(TEXT("NoCollision"));
	//TargetMarkerMesh->SetGenerateOverlapEvents(false);
	//TargetMarkerMesh->SetUsingAbsoluteRotation(true);

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	//if (PlaneMesh.Succeeded())
	//{
	//	TargetMarkerMesh->SetStaticMesh(PlaneMesh.Object);
	//}
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> TargetMat(TEXT("/Game/etc/Material/M_Target1.M_Target1"));
	//if (TargetMat.Succeeded())
	//{
	//	TargetMarkerMesh->SetMaterial(0, TargetMat.Object);
	//}
	//else
	//{
	//	LR_WARN(TEXT("타겟 머티리얼을 찾을 수 없음! 경로 확인 필요."));
	//}

	//TargetMarkerMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	//TargetMarkerMesh->SetRelativeScale3D(FVector(0.33f, 0.33f, 0.33f));



	//TargetMarkerMesh->SetVisibility(false);

}

void ALREnemyCharacter::OnDie()
{
	// OnDie() 단일 호출
	if (!IsDead)
	{
		IsDead = true;
	}
	else
	{
		return;
	}

	// TODO: 사망 몽타주 중단되는 경우 디버그
	//if (AbilitySystemComponent)
	//{
	//	// 필요하다면 모든 GameplayEffect 제거
	//	FGameplayTagContainer EffectsToRemove;
	//	EffectsToRemove.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Shock")));
	//	AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectsToRemove);
	//	AbilitySystemComponent->CancelAllAbilities();
	//}

	//LR_ERROR(TEXT("======================%s의 OnDie() 함수 실행됨.======================"), *GetName());

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

	// 4. 사망 몽타주 재생 => 이후 스켈레탈 메시 해제 & 풀 반환 OnDeathMontageEnded -> FinishDeathSequence()에서 처리
	PlayDeathMontage();
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

	// BehaviorTree 설정 및 실행
	if (EnemyData.BehaviorTree)
	{
		UBehaviorTree* BT = EnemyData.BehaviorTree.Get();

		if (BT)
		{
			if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
			{
				if (ALREnemyAIController* EnemyAICtrl = Cast<ALREnemyAIController>(AICtrl))
				{
					EnemyAICtrl->InitializeFromEnemyData(EnemyID);
				}

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

	// (260316) BJM: 몹 크기에 맞춰 화살표 위치를 캡슐 정수리 위로 자동 조절
	if (TargetMarkerMesh && GetCapsuleComponent())
	{
		float ZOffset = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		TargetMarkerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, ZOffset));
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
	AttributeSet->InitAttackPower(static_cast<float>(EnemyData.Attack));
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

void ALREnemyCharacter::ApplyVisualData(const FEnemyStaticData& EnemyData)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		LR_WARN(TEXT("SkeletalMeshComponent not found on Enemy [%s]"), *CurrentEnemyID.ToString());
		return;
	}

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

	if (!EnemyData.DeathMontage.IsNull())
	{
		CachedDeathMontage = EnemyData.DeathMontage.LoadSynchronous();
		if (!CachedDeathMontage)
		{
			LR_WARN(TEXT("Failed to Load Death Montage [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		}
	}
	else
	{
		CachedDeathMontage = nullptr;
		LR_WARN(TEXT("No Vaild Death Montage In DT [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
	}

	if (!EnemyData.AttackedMontage.IsNull())
	{
		CachedAttackedMontage = EnemyData.AttackedMontage.LoadSynchronous();
		if (!CachedAttackedMontage)
		{
			LR_WARN(TEXT("Failed to Load Attacked Montage [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		}
	}
	else
	{
		CachedAttackedMontage = nullptr;
		LR_WARN(TEXT("No Vaild Attakced Montage In DT [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
	}
}

void ALREnemyCharacter::PlayAttackedMontage()
{
	if (IsDead)
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		LR_WARN(TEXT("SKM 컴포넌트에 피격 몽타주 X [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		LR_WARN(TEXT("AnimInstance 피격 몽타주 X [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		return;
	}

	if (!CachedAttackedMontage)
	{
		LR_WARN(TEXT("캐싱된 피격 몽타주 X [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		return;
	}

	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ALREnemyCharacter::OnAttackedMontageEnded);
	AnimInstance->OnMontageEnded.AddDynamic(this, &ALREnemyCharacter::OnAttackedMontageEnded);

	float MontageLength = AnimInstance->Montage_Play(
		CachedAttackedMontage,
		1.0f,
		EMontagePlayReturnType::MontageLength,
		0.0f,
		true  // bStopAllMontages => 공격 몽타주보다 높은 우선 순위
	);
}

void ALREnemyCharacter::PlayDeathMontage()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		LR_ERROR(TEXT("SKM not found in PlayDeathMontage [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		FinishDeathSequence();
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	if (!AnimInstance)
	{
		LR_ERROR(TEXT("AnimInstance not found in PlayDeathMontage [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		FinishDeathSequence();
		return;
	}

	if (!CachedDeathMontage)
	{
		LR_WARN(TEXT("No DeathMontage cached. [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		FinishDeathSequence();
		return;
	}

	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ALREnemyCharacter::OnDeathMontageEnded);
	AnimInstance->OnMontageEnded.AddDynamic(this, &ALREnemyCharacter::OnDeathMontageEnded);

	float MontageLength = AnimInstance->Montage_Play(
		CachedDeathMontage,
		1.0f,                                    // PlayRate
		EMontagePlayReturnType::MontageLength,
		0.0f,                                    // StartTime
		true                                     // bStopAllMontages
	);

	if (MontageLength <= 0.0f)
	{
		LR_ERROR(TEXT("Failed to play DeathMontage for  [ID : %s] / [Name : %s]"), *CurrentEnemyID.ToString(), *GetName());
		FinishDeathSequence();
	}
	else
	{
		return;
	}
}

void ALREnemyCharacter::OnAttackedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &ALREnemyCharacter::OnAttackedMontageEnded);
	}
}

void ALREnemyCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &ALREnemyCharacter::OnDeathMontageEnded);
	}

	if (bInterrupted)
	{
		// TODO: 사망 몽타주 중단되는 원인 찾기
		//LR_ERROR(TEXT("======= [%s] Death montage interrupted ======="), *CurrentEnemyID.ToString());
	}

	FinishDeathSequence();
}

void ALREnemyCharacter::FinishDeathSequence()
{
	// 기존 OnDie() 4번 이후 플로우
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetSkeletalMesh(nullptr);
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetVisibility(false);
	}

	UPoolingSubsystem* PoolSys = GetWorld() ? GetWorld()->GetSubsystem<UPoolingSubsystem>() : nullptr;
	if (!PoolSys)
	{
		LR_ERROR(TEXT("PoolingSubsystem not found while returning enemy to pool"));
		return;
	}

	PoolSys->ReturnToPool(this);
}

float ALREnemyCharacter::GetDropAetherAmount() const
{
	if (CurrentEnemyID == NAME_None)
	{
		LR_WARN(TEXT("Not Valid CurrentEnemyID : %s || %s"), *GetName(), *CurrentEnemyID.ToString());
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

	IsDead = false;

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

	CachedDeathMontage = nullptr;
	CachedAttackedMontage = nullptr;

	CurrentEnemyID = NAME_None;
}

// (260316) BJM: 타겟 마커 스위치 함수
void ALREnemyCharacter::SetTargetMarkerVisibility(bool bVisible)
{
	if (TargetMarkerMesh)
	{
		TargetMarkerMesh->SetVisibility(bVisible);
	}
}

//(260316) BJM: 타겟팅 마커(머리 위 화살표) UI 연동을 위한 컴포넌트 설정
void ALREnemyCharacter::SetupTargetMarker()
{
	TargetMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMarkerMesh"));
	TargetMarkerMesh->SetupAttachment(RootComponent);

	TargetMarkerMesh->SetCollisionProfileName(TEXT("NoCollision"));
	TargetMarkerMesh->SetGenerateOverlapEvents(false);
	TargetMarkerMesh->SetUsingAbsoluteRotation(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		TargetMarkerMesh->SetStaticMesh(PlaneMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TargetMat(TEXT("/Game/etc/Material/M_Target1.M_Target1"));
	if (TargetMat.Succeeded())
	{
		TargetMarkerMesh->SetMaterial(0, TargetMat.Object);
	}
	else
	{
		LR_WARN(TEXT("타겟 머티리얼을 찾을 수 없음! 경로 확인 필요."));
	}

	TargetMarkerMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	TargetMarkerMesh->SetRelativeScale3D(FVector(0.33f, 0.33f, 0.33f));



	TargetMarkerMesh->SetVisibility(false);
}
