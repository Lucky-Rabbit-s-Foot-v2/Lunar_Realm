// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/LRGameplayAbilityBase.h"

#include "Logging/TokenizedMessage.h"
#include "Units/LRCharacter.h"

ULRGameplayAbilityBase::ULRGameplayAbilityBase()
{
	//기본 인스턴스 정책은 액터별 처리
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//멀티플레이 어빌리티 실행 정책은 클라이언트 예측 실행 후 서버 확정 방식
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void ULRGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData)
	{
		CachedInstigator = Cast<ALRCharacter>(TriggerEventData->Instigator.Get());
		CachedTarget = TriggerEventData->Target.Get();
	}
	
	OnAbilityActivated(Handle, ActorInfo, ActivationInfo);
}

void ULRGameplayAbilityBase::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	//자식 GA들의 실질적인 로직 구성.
}

ALRCharacter* ULRGameplayAbilityBase::GetCharacterFromActorInfo(const FGameplayAbilityActorInfo& ActorInfo) const
{
	return Cast<ALRCharacter>(ActorInfo.AvatarActor.Get());
}


UAbilitySystemComponent* ULRGameplayAbilityBase::GetOwnerASC() const
{
	return GetAbilitySystemComponentFromActorInfo_Ensured();
}
