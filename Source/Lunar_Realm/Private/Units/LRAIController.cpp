// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/LRAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "GAS/Tags/LRGameplayTags.h"
#include "GameplayTagAssetInterface.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Structures/Core/LRCore.h"
#include "System/LoggingSystem.h"

#include "Units/LRCharacter.h"


ALRAIController::ALRAIController()
{
}


void ALRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn)
	{
		return;
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


bool ALRAIController::TryAttackTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastAttackTime < AttackCooldown)
	{
		return false;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return false;
	}

	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(MyPawn))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
		{
			FGameplayTagContainer AttackTags;
			AttackTags.AddTag(LRTags::Ability_Combat_BasicShoot);

			if (ASC->TryActivateAbilitiesByTag(AttackTags))
			{
				LastAttackTime = CurrentTime;
				return true;
			}

			LR_WARN(TEXT("[%s] No attack ability on [%s]."),
				*GetName(), *MyPawn->GetName());
			LastAttackTime = CurrentTime;
		}
	}

	return false;
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


