// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Enemy/LRGA_InstantAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Units/LRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_InstantAttack::ULRGA_InstantAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// 
	AbilityTags.AddTag(LRTags::Ability_Combat_BasicShoot);
}

void ULRGA_InstantAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 1. 타겟 찾기 (Controller에서!)
	AActor* TargetActor = nullptr;

	if (ActorInfo && ActorInfo->AvatarActor.IsValid())
	{
		APawn* OwnerPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
		if (OwnerPawn)
		{
			ALRAIController* AIController = Cast<ALRAIController>(OwnerPawn->GetController());
			if (AIController)
			{
				UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
				if (BlackboardComp)
				{
					TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
				}
			}
		}
	}

	if (!TargetActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 2. 거리 체크
	float Distance = FVector::Dist(ActorInfo->AvatarActor->GetActorLocation(),
		TargetActor->GetActorLocation());
	if (Distance > AttackRange)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 3. 타겟의 ASC 가져오기
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC || !DamageEffectClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 4. GameplayEffect 즉시 적용!
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(ActorInfo->AvatarActor.Get());

	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(
		DamageEffectClass, 1.0f, EffectContext);

	if (SpecHandle.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}

	// 5. 즉시 종료!
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
