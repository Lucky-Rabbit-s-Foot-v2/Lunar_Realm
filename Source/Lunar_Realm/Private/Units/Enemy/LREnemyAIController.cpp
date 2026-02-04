// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Structures/Core/LRCore.h"
#include "GameplayTagAssetInterface.h"


ALREnemyAIController::ALREnemyAIController()
{
	if (!TargetBaseTag.IsValid())
	{
		TargetBaseTag = FGameplayTag::RequestGameplayTag(FName("Test.KWB.Base"));
	}
}

void ALREnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MoveToBase();
}

AActor* ALREnemyAIController::FindActorWithGameplayTag(const FGameplayTag& Tag) const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALRCore::StaticClass(), FoundActors);

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

void ALREnemyAIController::MoveToBase()
{
	AActor* TargetActor = FindActorWithGameplayTag(TargetBaseTag);
	if (!TargetActor)
	{
		return;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(TargetActor);
	MoveRequest.SetAcceptanceRadius(50.0f);
	MoveRequest.SetUsePathfinding(true);

	MoveTo(MoveRequest);
}
