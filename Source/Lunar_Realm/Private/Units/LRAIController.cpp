// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/LRAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "GAS/Tags/LRGameplayTags.h"
#include "GameplayTagAssetInterface.h"

#include "Navigation/CrowdFollowingComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Structures/Core/LRCore.h"
#include "System/LoggingSystem.h"

#include "Units/LRCharacter.h"


ALRAIController::ALRAIController()
{
}


void ALRAIController::OnPoolActivate_Implementation()
{
	SetActorTickEnabled(true);
}

void ALRAIController::OnPoolDeactivate_Implementation()
{
	// 1. BehaviorTree 중단
	if (UBehaviorTreeComponent* BTComp = FindComponentByClass<UBehaviorTreeComponent>())
	{
		BTComp->StopTree(EBTStopMode::Safe);
	}

	// 2. Blackboard 정리
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->ClearValue(LRBBKeys::TargetActor);
		BB->ClearValue(LRBBKeys::HasNearbyHostile);
		// TargetCore는 유지 (어차피 다시 같은 값)
	}

	// 3. Pawn 연결 해제
	if (GetPawn())
	{
		UnPossess();
	}

	// 4. 비활성화
	SetActorTickEnabled(false);
}

void ALRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn)
	{
		return;
	}

	if (UCrowdFollowingComponent* CrowdComp =
		Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComp->SetCrowdSimulationState(ECrowdSimulationState::Enabled);

		CrowdComp->SetCrowdObstacleAvoidance(true, true);
		CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High, true);

		CrowdComp->SetCrowdSeparation(true, true);
		CrowdComp->SetCrowdSeparationWeight(250.f, true); // 필요시 해당 수치 조절

		CrowdComp->SetCrowdAnticipateTurns(true, true);
		CrowdComp->SetCrowdOptimizeVisibility(true, true);
		CrowdComp->SetCrowdOptimizeTopology(true, true);

		CrowdComp->SetCrowdCollisionQueryRange(600.0f, true);
		CrowdComp->SetCrowdAvoidanceRangeMultiplier(1.2f, true);

		CrowdComp->SetCrowdSlowdownAtGoal(false, true);
	}
	else
	{
		LR_WARN(TEXT("[%s] CrowdFollowingComponent Cast 실패"), *GetName());
	}

	// 태그 유효성 검사
	if (!HostileRootTag.IsValid())
	{
		LR_ERROR(TEXT("[%s] HostileRootTag is not set."), *GetName());
		return;
	}
	if (!TargetCoreTag.IsValid())
	{
		LR_ERROR(TEXT("[%s] TargetCoreTag is not set."), *GetName());
		return;
	}
}


void ALRAIController::OnUnPossess()
{
	StopMovement();
	Super::OnUnPossess();
}

// ex. LREnemyController에서는 "LRTags::Team_Player"
FGameplayTag ALRAIController::GetHostileRootTag() const
{
	return HostileRootTag;
}

// ex. LRTags::Team_Player_Structure_Core
FGameplayTag ALRAIController::GetTargetCoreTag() const
{
	return TargetCoreTag;
}


// DetectionRadius 내 HostileRootTag 계열 태그를 가진 가장 가까운 LRCharacter 반환.
// MatchesTag() 계층 매칭: "Player"로 검사하면 Player.Character.Player/Member 모두 감지.
AActor* ALRAIController::FindNearestHostile() const
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return nullptr;
	}

	const FGameplayTag HostileTag = GetHostileRootTag();
	if (!HostileTag.IsValid())
	{
		return nullptr;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> OverlapResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MyPawn);

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		MyPawn->GetActorLocation(),
		DetectionRadius,
		ObjectTypes,
		ALRCharacter::StaticClass(),
		ActorsToIgnore,
		OverlapResults
	);

	AActor* Closest = nullptr;
	float MinDistSq = FLT_MAX;

	for (AActor* Candidate : OverlapResults)
	{
		ALRCharacter* LRChar = Cast<ALRCharacter>(Candidate);
		if (!LRChar || !LRChar->GetUnitTag().IsValid())
		{
			continue;
		}

		if (!LRChar->GetUnitTag().MatchesTag(HostileTag))
		{
			continue;
		}

		// State.Dead 태그를 가진 캐릭터는 제외
		if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(LRChar))
		{
			if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
			{
				if (ASC->HasMatchingGameplayTag(LRTags::State_Dead))
				{
					continue;
				}
			}
		}

		const float DistSq = FVector::DistSquared(MyPawn->GetActorLocation(), LRChar->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Closest = LRChar;
		}
	}

	return Closest;
}


AActor* ALRAIController::FindTargetCore() const
{
	const FGameplayTag CoreTag = GetTargetCoreTag();
	if (!CoreTag.IsValid())
	{
		return nullptr;
	}

	return FindActorWithGameplayTag(ALRCore::StaticClass(), CoreTag);
}


AActor* ALRAIController::FindActorWithGameplayTag(
	TSubclassOf<AActor> ActorClass, const FGameplayTag& Tag) const
{
	if (!ActorClass || !Tag.IsValid())
	{
		return nullptr;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!Actor)
		{
			continue;
		}

		if (const IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Actor))
		{
			FGameplayTagContainer OwnedTags;
			TagInterface->GetOwnedGameplayTags(OwnedTags);

			if (OwnedTags.HasTagExact(Tag))
			{
				return Actor;
			}
		}
	}

	return nullptr;
}


FGameplayTag ALRAIController::TryAttackTarget(AActor* Target)
{
	if (!Target)
	{
		LR_WARN(TEXT("[%s] : TryAttackTarget 시도, Target이 없습니다."), *GetName());
		return FGameplayTag();
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		LR_WARN(TEXT("[%s] : TryAttackTarget 시도, MyPawn이 없습니다."), *GetName());
		return FGameplayTag();
	}
	
	//260219 KHS 태그 기반이 아닌 이벤트 데이터 방식 GA발동 방식으로 변경
	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(MyPawn);
	if (!OwnerCharacter)
	{
		LR_WARN(TEXT("[%s] : TryAttackTarget 시도, OwnerCharacter이 없습니다."), *GetName());
		return FGameplayTag();
	}
	
	FGameplayTag SkillTag = LRTags::Ability_Combat_BasicShoot;

	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = Target;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter, 
		SkillTag,
		EventData);
	
	return SkillTag;
}

void ALRAIController::InitializeBehaviorTree(UBehaviorTree* NewBT)
{
	if (!NewBT)
	{
		LR_ERROR(TEXT("[%s] IntializeBehaviorTree: NewBT is NULL."), *GetName());
		return;
	}

	BehaviorTreeAsset = NewBT;

	// BT 실행
	if (!RunBehaviorTree(BehaviorTreeAsset))
	{
		LR_ERROR(TEXT("[%s] IntializeBehaviorTree() failed."), *GetName());
		return;
	}

	// BB 초기값: 코어 캐싱
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		AActor* CoreActor = FindTargetCore();
		BB->SetValueAsObject(LRBBKeys::TargetCore, CoreActor);

		if (!CoreActor)
		{
			LR_WARN(TEXT("[%s] Target Core not found. 레벨에 [%s] 태그를 가진 코어가 필요합니다."),
				*GetName(), *TargetCoreTag.ToString());
		}
	}
}


