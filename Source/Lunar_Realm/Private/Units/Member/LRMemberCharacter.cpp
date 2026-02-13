// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Member/LRMemberCharacter.h"
#include "Animation/AnimInstance.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "AIController.h" 
#include "System/LoggingSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Engine/World.h"
#include "TimerManager.h"

#include "Units/LRAIController.h"      
#include "Units/Member/LRMemberAIController.h"
#include "BehaviorTree/BehaviorTree.h"  

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
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				ULRPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRMemberCharacter::OnHealthChangedNative);
		}
	}
}

void ALRMemberCharacter::OnHealthChangedNative(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;

	if (bIsDead)
	{
		return;
	}
	if (NewHealth <= 0.0f)
	{
		Die();
	}

	OnHealthChanged(Data.NewValue);
}

void ALRMemberCharacter::OnPoolActivate_Implementation()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			// 현재 재생 중인 모든 몽타주(사망 모션 등)를 즉시 정지
			AnimInst->StopAllMontages(0.0f);
		}

		// 메시가 캡슐 밖으로 나갔을 수도 있으니 위치/회전 원상복구 (기본값)
		// (종민님 프로젝트 세팅에 맞는 값으로 설정하세요. 보통은 아래와 같습니다)
		MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
		MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	// 2. [핵심] 물리 및 이동 상태 리셋 (죽은 위치로 튕겨감 방지)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately(); // 이동 정지
		MoveComp->Velocity = FVector::ZeroVector; // 속도 0
		MoveComp->SetMovementMode(MOVE_Walking); // 걷기 모드로 강제 전환
		MoveComp->SetActive(true);
		MoveComp->bWantsToCrouch = false; // 혹시 웅크린 상태였다면 해제
	}

	// 3. 액터 상태 활성화
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsDead = false; // 사망 플래그 해제

	// 4. 캡슐 충돌 켜기 (물리 연산 포함)
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		// 죽었을 때 NoCollision이었던 것을 Pawn으로 복구
		Capsule->SetCollisionProfileName(TEXT("Pawn"));
	}

	// 5. 스탯 및 AI 초기화
	ResetAttributes();
	ResetAIController();

	/*if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->Velocity = FVector::ZeroVector;
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->SetActive(true);
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsDead = false;

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}


	ResetAttributes();
	ResetAIController();*/

	LR_INFO(TEXT("Member 풀에서 소환 및 초기화 완료 : %s"), *GetName());
}

void ALRMemberCharacter::OnPoolDeactivate_Implementation()
{

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (AController* AICon = GetController())
	{
		AICon->StopMovement();
		AICon->UnPossess();
	}
}

void ALRMemberCharacter::InitCharacterData(FName InCharacterID)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return;

	const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(InCharacterID);

	if (CharData.DataID == NAME_None)
	{
		LR_ERROR(TEXT("DT에서 ID [%s]를 찾을 수 없음"), *InCharacterID.ToString());
		return;
	}

	// 캐릭터 매쉬 로드 및 캐싱
	if (!CharData.CharacterMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = CharData.CharacterMesh.LoadSynchronous();
		if (LoadedMesh && GetMesh())
		{
			GetMesh()->SetSkeletalMeshAsset(LoadedMesh);
			LR_INFO(TEXT("[%s] 메시 적용 성공!"), *InCharacterID.ToString());
		}
		else
		{
			LR_WARN(TEXT("[%s] 메시 로드 실패."), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] 데이터 테이블에 CharacterMesh가 빔!"), *InCharacterID.ToString());
	}

	// 애님 블루프린트 로드 및 캐싱
	if (!CharData.AnimBlueprintClass.IsNull())
	{
		TSubclassOf<UAnimInstance> LoadedAnim = CharData.AnimBlueprintClass.LoadSynchronous();
		if (LoadedAnim && GetMesh())
		{
			GetMesh()->SetAnimInstanceClass(LoadedAnim);
			LR_INFO(TEXT("[%s] ABP 적용 성공"), *InCharacterID.ToString());
		}
		else
		{ 
			LR_WARN(TEXT("[%s] ABP 로드 실패"), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] DT에 AnimBlueprintClass가 비어있음"), *InCharacterID.ToString());
	}

	// 사망 몽타주 로드 및 캐싱
	if (!CharData.DeathMontage.IsNull())
	{
		LoadedDeathMontage = CharData.DeathMontage.LoadSynchronous();
		if (LoadedDeathMontage)
		{
			LR_INFO(TEXT("[%s] 사망 몽타주 로드 성공"), *InCharacterID.ToString());
		}
	}
	else
	{
		LoadedDeathMontage = nullptr;
		LR_WARN(TEXT("[%s] DT에 사망 몽타주가 비어있습니다. (기본값 사용 주의)"), *InCharacterID.ToString());
	}

	// TODO : 공격 몽타주 추가
	// 공격 몽타주 로드 및 캐싱
	if (!CharData.NormalAttackMontage.IsNull())
	{
		LoadedAttackMontage = CharData.NormalAttackMontage.LoadSynchronous();
		LR_INFO(TEXT("[%s] 공격 몽타주 로드 성공"), *InCharacterID.ToString());
	}
	else
	{
		LoadedAttackMontage = nullptr;
	}

	// Behavior Tree 실행 로직
	if (CharData.BehaviorTree)
	{
		// 컨트롤러가 없으면 강제로 만듦
		if (GetController() == nullptr)
		{
			SpawnDefaultController();
		}

		// ALRAIController로 캐스팅해서 BT 실행
		if (ALRAIController* AIC = Cast<ALRAIController>(GetController()))
		{
			AIC->InitializeBehaviorTree(CharData.BehaviorTree);

			// AIC->RestartAI(); 

			LR_INFO(TEXT("[%s] BT 실행 성공: %s"), *InCharacterID.ToString(), *CharData.BehaviorTree->GetName());
		}
		else
		{
			LR_WARN(TEXT("[%s] Controller가 ALRAIController가 아님"), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] DT에 BehaviorTree가 없음."), *InCharacterID.ToString());
	}

	// (참고: 필요하면 여기서 CharData의 스탯을 이용해 체력/공격력 세팅을 추가할 수도 있음)
	LR_INFO(TEXT("[%s] 캐릭터 데이터 세팅 완료"), *InCharacterID.ToString());
}

void ALRMemberCharacter::ResetAttributes()
{
	if (MemberAttributeSet)
	{
		float MaxHP = MemberAttributeSet->GetMaxHealth();
		if (MaxHP <= 0.0f)
		{
			MaxHP = 50.0f;
		}

		MemberAttributeSet->InitHealth(MaxHP);
		MemberAttributeSet->InitMaxHealth(MaxHP);
		MemberAttributeSet->InitAttackPower(5.0f);
		
	}
}
void ALRMemberCharacter::ResetAIController()
{
	if (Controller == nullptr && AIControllerClass)
	{
		SpawnDefaultController();
	}
}

void ALRMemberCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	LR_INFO(TEXT("맴버 사망 : %s"), *GetName());

	if (Controller)
	{
		Controller->StopMovement();
		Controller->UnPossess();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	float ReturnDelay = 3.0f;
	if (LoadedDeathMontage)
	{
		float Duration = PlayAnimMontage(LoadedDeathMontage);

		if (Duration > 0.0f)
		{
			ReturnDelay = Duration + 0.5f;
		}
	}
	else
	{
		LR_WARN(TEXT("재생할 사망 몽타주(LoadedDeathMontage)가 없습니다! DT를 확인하세요."));
	}

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ALRMemberCharacter::ReturnSelf, ReturnDelay, false);
}


void ALRMemberCharacter::ReturnSelf()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	UPoolingSubsystem* PoolSys = World->GetSubsystem<UPoolingSubsystem>();
	if (PoolSys)
	{
		PoolSys->ReturnToPool(this);
		LR_INFO(TEXT("%s 풀 반환 완료"), *GetName());
	}
	else
	{
		Destroy();
		LR_INFO(TEXT("%s 풀 반환 실패"), *GetName());
	}
}
