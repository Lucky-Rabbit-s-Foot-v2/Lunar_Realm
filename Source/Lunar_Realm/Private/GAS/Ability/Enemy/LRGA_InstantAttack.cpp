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

	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Combat_BasicShoot);
	SetAssetTags(TempTags);
	
	//(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Combat_BasicShoot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillEffectID = "EFFECT_INSTANT_ATTACK";
	SkillID = "SKILL_INSTANT_ATTACK";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot.Cooldown")));
}


void ULRGA_InstantAttack::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	// 1. 타겟 찾기 (LRGameplayAbilityBase를 통해 캐싱된 타겟으로 설정)
	const AActor* TargetActor = Cast<const AActor>(CachedTarget);
	// TEST
	LR_INFO(TEXT("[InstantAttack] TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

	if (!TargetActor)
	{
		LR_WARN(TEXT("타겟 없음!!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 2. 타겟의 ASC 가져오기
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(const_cast<AActor*>(TargetActor));
	// TEST
	LR_INFO(TEXT("[InstantAttack] TargetASC: %s"), TargetASC ? TEXT("Valid") : TEXT("NULL"));
	if (!TargetASC || !DamageEffectClass)
	{
		LR_WARN(TEXT("타켓 Asc 못 찾음!!"));
		// TEST
		LR_WARN(TEXT("[InstantAttack] 실패 - ASC: %s / DamageEffect: %s"),
			TargetASC ? TEXT("Valid") : TEXT("NULL"),
			DamageEffectClass ? TEXT("Valid") : TEXT("NULL"));

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 3. GameplayEffect 즉시 적용!
	// (260310) BJM_수정 : TargetASC -> SourceASC로 변경
	UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
	if (!SourceASC) return;

	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(ActorInfo->AvatarActor.Get());
	EffectContext.AddInstigator(ActorInfo->AvatarActor.Get(), ActorInfo->AvatarActor.Get());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, 1.0f, EffectContext);

	if (SpecHandle.IsValid())
	{
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}

	// 4. 즉시 종료!
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
