// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyNormalCharacter.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "GAS/Tags/LRGameplayTags.h"


ALREnemyNormalCharacter::ALREnemyNormalCharacter()
{
	AIControllerClass = ALREnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    UnitTag = LRTags::Team_Enemy_Character_Normal;
}
