// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyBossCharacter.h"

#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/Stage/LRStageGameMode.h"
#include "Data/LRDataStructs.h"
#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"

ALREnemyBossCharacter::ALREnemyBossCharacter()
{
	AIControllerClass = ALREnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// 보스 캡슐 사이즈 조절용
	GetCapsuleComponent()->SetCapsuleSize(350.f, 200.f); // SetCapsuleSize(float InRadius, float InHalfHeight)

	// 겹쳐서 날아가는 것 방지
	GetCharacterMovement()->Mass = 10000.f;
	GetCharacterMovement()->bEnablePhysicsInteraction = false;

	UnitTag = LRTags::Team_Enemy_Character_Boss;

	CoreAttackOverlap = CreateDefaultSubobject<USphereComponent>(TEXT("CoreAttackOverlap"));
	CoreAttackOverlap->SetupAttachment(RootComponent);
	CoreAttackOverlap->SetSphereRadius(1100.f);
	CoreAttackOverlap->SetCollisionProfileName(TEXT("OverlapAll"));
}

void ALREnemyBossCharacter::InitializeBossSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = SpeedPhase0;
}

void ALREnemyBossCharacter::SetCoreAttackOverlapRadius(float InRadius)
{
	if (CoreAttackOverlap)
	{
		CoreAttackOverlap->SetSphereRadius(InRadius);
	}
}

void ALREnemyBossCharacter::RegisterMontageNotifyDelegate()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		LR_ERROR(TEXT("[Boss] MeshComp NULL"));
		return;
	}

	UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
	if (!AnimInst)
	{
		LR_ERROR(TEXT("[Boss] AnimInst NULL"));
		return;
	}

	AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &ALREnemyBossCharacter::OnMontageNotifyStart);
}

void ALREnemyBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 보스는 DetourCrowd로 다른 에너미 안피하게 설정
	if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
	{
		if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(AICtrl->GetPathFollowingComponent()))
		{
			CrowdComp->SetCrowdSimulationState(ECrowdSimulationState::ObstacleOnly);
		}
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("[Boss] ASC를 찾을 수 없음"));
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(ULREnemyAttributeSet::GetHealthAttribute()).AddUObject(this, &ALREnemyBossCharacter::OnBossHealthChanged);

	CoreAttackOverlap->OnComponentBeginOverlap.AddDynamic(this, &ALREnemyBossCharacter::OnCoreOverlapBegin);
	CoreAttackOverlap->OnComponentEndOverlap.AddDynamic(this, &ALREnemyBossCharacter::OnCoreOverlapEnd);
}

void ALREnemyBossCharacter::FinishDeathSequence()
{
	Super::FinishDeathSequence();

	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		StageGM->OnGameClear();
	}
	else
	{
		LR_ERROR(TEXT("[Boss] StageGameMode를 찾을 수 없음 - GameClear 호출 실패"));
	}
}

void ALREnemyBossCharacter::ApplyVisualData(const FEnemyStaticData& EnemyData)
{
	Super::ApplyVisualData(EnemyData);

	// 보스 전용: DT 기반 캡슐 사이즈 적용
	if (EnemyData.CapsuleRadius > KINDA_SMALL_NUMBER && EnemyData.CapsuleHalfHeight > KINDA_SMALL_NUMBER)
	{
		GetCapsuleComponent()->SetCapsuleSize(EnemyData.CapsuleRadius, EnemyData.CapsuleHalfHeight);
	}
}

void ALREnemyBossCharacter::OnMontageNotifyStart(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
{
	if (NotifyName != "GameOver")
	{
		return;
	}

	ALRAIController* AICtrl = Cast<ALRAIController>(GetController());
	if (!AICtrl)
	{
		return;
	}

	UBlackboardComponent* BB = AICtrl->GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	if (!BB->GetValueAsBool(LRBBKeys::CoreInRange))
	{
		return;
	}

	LR_WARN(TEXT("[Boss] GameOver 노티파이 감지 - 코어 파괴 시작"));

	AActor* FoundCore = UGameplayStatics::GetActorOfClass(GetWorld(), ALRPlayerCore::StaticClass());
	if (ALRPlayerCore* PlayerCore = Cast<ALRPlayerCore>(FoundCore))
	{
		PlayerCore->OnCoreDestroyed();
	}
	else
	{
		LR_ERROR(TEXT("[Boss] PlayerCore를 찾을 수 없음"));
	}
}

void ALREnemyBossCharacter::OnCoreOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<ALRPlayerCore>(OtherActor))
	{
		return;
	}

	if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICtrl->GetBlackboardComponent())
		{
			BB->SetValueAsBool(LRBBKeys::CoreInRange, true);
		}
	}
}

void ALREnemyBossCharacter::OnCoreOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Cast<ALRPlayerCore>(OtherActor)) return;

	if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICtrl->GetBlackboardComponent())
		{
			BB->SetValueAsBool(LRBBKeys::CoreInRange, false);
		}
	}
}

void ALREnemyBossCharacter::OnBossHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("[Boss] ASC를 찾을 수 없음"));
		return;
	}

	const float MaxHealth = ASC->GetNumericAttribute(ULREnemyAttributeSet::GetMaxHealthAttribute());
	if (MaxHealth <= 0.0f)
	{
		return;
	}

	const float HealthPercent = Data.NewValue / MaxHealth;
	const int32 NewPhase = CalculatePhase(HealthPercent);

	if (NewPhase != CurrentPhase)
	{
		CurrentPhase = NewPhase;

		const float PhaseSpeedTable[] = { SpeedPhase0, SpeedPhase1, SpeedPhase2 };
		if (CurrentPhase < UE_ARRAY_COUNT(PhaseSpeedTable))
		{
			GetCharacterMovement()->MaxWalkSpeed = PhaseSpeedTable[CurrentPhase];
		}

		if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICtrl->GetBlackboardComponent())
			{
				BB->SetValueAsInt(LRBBKeys::CurrentPhase, CurrentPhase);
				ActivateAuraVFX(CurrentPhase);
			}
			else
			{
				LR_ERROR(TEXT("[Boss] BB 찾을 수 없음"));
			}
		}
		else
		{
			LR_ERROR(TEXT("[Boss] 컨트롤러 찾을 수 없음"));
		}
	}
}

int32 ALREnemyBossCharacter::CalculatePhase(float HealthPercent) const
{
	for (int32 i = 0; i < PhaseThresholds.Num(); ++i)
	{
		if (HealthPercent > PhaseThresholds[i])
		{
			return i;
		}
	}
	return PhaseThresholds.Num();
}
