// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyNormalCharacter.h"
#include "Units/Enemy/LREnemyAIController.h"
#include "GAS/Tags/LRGameplayTags.h"


ALREnemyNormalCharacter::ALREnemyNormalCharacter()
{
	AIControllerClass = ALREnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::Disabled;

    UnitTag = LRTags::Team_Enemy_Character_Normal;
}

void ALREnemyNormalCharacter::OnPoolActivate_Implementation()
{
	// ASC에서 필요하니 등록 전 값 설정 필요 
	UnitTag = LRTags::Team_Enemy_Character_Normal;

	Super::OnPoolActivate_Implementation();

	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->AddLooseGameplayTag(UnitTag);
	}
}

void ALREnemyNormalCharacter::OnPoolDeactivate_Implementation()
{
	if (GetAbilitySystemComponent() && UnitTag.IsValid())
	{
		GetAbilitySystemComponent()->RemoveLooseGameplayTag(UnitTag);
	}

	Super::OnPoolDeactivate_Implementation();

	UnitTag = FGameplayTag();
}
