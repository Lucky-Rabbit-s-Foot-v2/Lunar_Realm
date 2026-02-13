// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Member/LRMemberAIController.h"
#include "GAS/Tags/LRGameplayTags.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

ALRMemberAIController::ALRMemberAIController()
{
	HostileRootTag = LRTags::Team_Enemy;
	TargetCoreTag = LRTags::Team_Enemy_Structure_Core;
}

void ALRMemberAIController::RestartAI()
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		AActor* CoreActor = FindTargetCore();
		BB->SetValueAsObject(LRBBKeys::TargetCore, CoreActor);

		BB->SetValueAsBool(LRBBKeys::HasNearbyHostile, false);
		BB->SetValueAsObject(LRBBKeys::TargetActor, nullptr);
	}

	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}
