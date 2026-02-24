// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_BasicAttack.h"
#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Tags/LRGameplayTags.h"


ULRGA_BasicAttack::ULRGA_BasicAttack()
{
	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
	// FGameplayTagContainer Tags;
	// Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
	// SetAssetTags(Tags);

	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Combat_BasicShoot);
	SetAssetTags(TempTags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Combat_BasicShoot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}


void ULRGA_BasicAttack::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("[GA_Attack] 평타 GA 실행됨! 진입 성공!"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Attack] 실패: CommitAbility 통과 못함 (쿨타임이거나 코스트 부족)"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 공격자 정보
	ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*ActorInfo);
	//ALRCharacter* OwnerChar = Cast<ALRCharacter>(ActorInfo->AvatarActor.Get());
	if (!OwnerChar)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Attack] 실패: OwnerChar가 NULL임!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}


	// 타겟 정보
	const AActor* TargetActor = CachedTarget;
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA_Attack] 실패: CachedTarget이 NULL임! (이벤트 데이터 누락 또는 캐스팅 실패)"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//// 데미지 적용
	//if (DamageEffectClass && GetOwnerASC())
	//{
	//	FGameplayEffectContextHandle Context = GetOwnerASC()->MakeEffectContext();
	//	FGameplayEffectSpecHandle SpecHandle = GetOwnerASC()->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

	//	if (SpecHandle.IsValid())
	//	{
	//		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	//		if (TargetASC)
	//		{
	//			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	//			UE_LOG(LogTemp, Warning, TEXT("[GA_Attack] 데미지 GE 적용 성공! 타겟: %s"), *TargetActor->GetName());
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Error, TEXT("[GA_Attack] 타겟의 ASC를 찾을 수 없습니다! 타겟: %s"), *TargetActor->GetName());
	//		}
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("[GA_Attack] DamageEffectClass가 None이거나 내 ASC가 없습니다!"));
	//}

	if (!CachedTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("[GA] CachedTarget이 NULL입니다! 캐스팅 실패!"));
	}

	// 몽타주 적용
	if (AttackMontage)
	{
		//OwnerChar->PlayAnimMontage(AttackMontage);

		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);

		MontageTask->OnBlendOut.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);
		MontageTask->OnCompleted.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);

		MontageTask->ReadyForActivation();

	}

	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, LRTags::Notify_Combat_BasicHit);
	EventTask->EventReceived.AddDynamic(this, &ULRGA_BasicAttack::OnHitEventReceived);
	EventTask->ReadyForActivation();

	//EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	
	
	
}

void ULRGA_BasicAttack::OnHitEventReceived(FGameplayEventData InPayload)
{
	const AActor* TargetActor = CachedTarget;
	if (!TargetActor) return;

	if (DamageEffectClass && GetOwnerASC())
	{
		FGameplayEffectContextHandle Context = GetOwnerASC()->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = GetOwnerASC()->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			if (TargetASC)
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				UE_LOG(LogTemp, Warning, TEXT("노티파이 타이밍에 데미지 성공 타겟: %s"), *TargetActor->GetName());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DamageEffectClass가 None이거나 내 ASC가 없음"));
	}
}

void ULRGA_BasicAttack::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}