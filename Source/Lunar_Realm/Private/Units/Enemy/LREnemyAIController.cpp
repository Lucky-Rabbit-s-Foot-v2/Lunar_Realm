// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/GameDataSubsystem.h"
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

void ALREnemyAIController::InitializeFromEnemyData(FName EnemyID)
{
	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys)
	{
		LR_WARN(TEXT("[%s] InitializeFromEnemyData: No valid GameDataSubsystem"), *GetName());
		return;
	}

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);
	AttackRange = EnemyData.AttackRange;

	// TEST
	LR_INFO(TEXT("[%s] AttackRange 설정값: %f"), *EnemyID.ToString(), AttackRange);
}
