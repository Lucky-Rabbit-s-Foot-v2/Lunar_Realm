// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystemBlueprintLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "Units/LRCharacter.h"


ALREnemyAIController::ALREnemyAIController()
{
	HostileRootTag = LRTags::Team_Player;
	TargetCoreTag = LRTags::Team_Player_Structure_Core;
}

bool ALREnemyAIController::TryAttackTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return false;
	}

	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(MyPawn);
	if (!OwnerCharacter)
	{
		return false;
	}

	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = Target;

	// TODO: 추후 데이터 테이블에서 각각 태그 가져와서 스킬 달리 사용하도록 로직 리팩토링
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter,
		LRTags::Ability_Combat_BasicShoot,
		EventData);

	return true;
}
