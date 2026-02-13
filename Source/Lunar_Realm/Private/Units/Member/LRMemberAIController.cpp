// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Member/LRMemberAIController.h"
#include "GAS/Tags/LRGameplayTags.h"

ALRMemberAIController::ALRMemberAIController()
{
	HostileRootTag = LRTags::Team_Enemy;
	TargetCoreTag = LRTags::Team_Enemy_Structure_Core;
}
