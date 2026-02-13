// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GAS/Tags/LRGameplayTags.h"


ALREnemyAIController::ALREnemyAIController()
{
	HostileRootTag = LRTags::Team_Player;
	TargetCoreTag = LRTags::Team_Player_Structure_Core;
}