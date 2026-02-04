// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyNormalCharacter.h"
#include "Units/Enemy/LREnemyAIController.h"

ALREnemyNormalCharacter::ALREnemyNormalCharacter()
{
	AIControllerClass = ALREnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
