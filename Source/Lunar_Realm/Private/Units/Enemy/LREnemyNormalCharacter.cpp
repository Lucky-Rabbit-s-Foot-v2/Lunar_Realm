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
		// 중복 추가를 방지, 태그의 개수 1개로 강제 고정
		GetAbilitySystemComponent()->SetLooseGameplayTagCount(UnitTag, 1);
	}
}

void ALREnemyNormalCharacter::OnPoolDeactivate_Implementation()
{
	if (GetAbilitySystemComponent() && UnitTag.IsValid())
	{
		// 명시적으로 0개로 고정
		GetAbilitySystemComponent()->SetLooseGameplayTagCount(UnitTag, 0);
	}

	Super::OnPoolDeactivate_Implementation();

	UnitTag = FGameplayTag();
}
