// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_RapidSlash.h"

#include "DrawDebugHelpers.h"
#include "Units/LRCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


ULRGA_RapidSlash::ULRGA_RapidSlash()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_RapidSlash;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_RAPIDSLASH";
	SkillEffectID = "EFFECT_RAPIDSLASH";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.RapidSlash.Cooldown")));
}

void ULRGA_RapidSlash::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	LR_INFO(TEXT("[LRGA_RapidSlash] OnAbilityActivated 진입"));

	if (!CachedInstigator.IsValid() || !RapidMontage || !DamageEffectClass)
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	UGameInstance* GI = GetWorld()->GetGameInstance();
	check(GI);
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	CachedDamage = EffectData.Amount;
	CachedHitRadius = EffectData.Range > 0.f ? EffectData.Range : 200.0f;
	CachedResourceID = SkillData.ResourceID;

	FGameplayTag HostileTag = GetHostileTeamTag();
	LockedTarget = FindNearestHostile(HostileTag, 500.0f);

	CurrentStrikeCount = 0;
	StartNextStrike();

}

void ULRGA_RapidSlash::StartNextStrike()
{
	if (CurrentStrikeCount >= MaxStrikeCount)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*CurrentActorInfo);
	if (!OwnerChar) return;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, RapidMontage, 5.0f);

	MontageTask->OnBlendOut.AddDynamic(this, &ULRGA_RapidSlash::OnStrikeMontageFinished);
	MontageTask->OnCompleted.AddDynamic(this, &ULRGA_RapidSlash::OnStrikeMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &ULRGA_RapidSlash::OnStrikeMontageFinished);
	MontageTask->OnCancelled.AddDynamic(this, &ULRGA_RapidSlash::OnStrikeMontageFinished);

	MontageTask->ReadyForActivation();

	PerformStrikeLogic();
}

void ULRGA_RapidSlash::PerformStrikeLogic()
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerChar) return;

	FVector StrikeCenter;

	if (LockedTarget.IsValid())
	{
		StrikeCenter = LockedTarget->GetActorLocation();
	}
	else
	{
		StrikeCenter = OwnerChar->GetActorLocation() + (OwnerChar->GetActorForwardVector() * 300.0f);
	}

	// 적을 때리기 전에 ResourceData를 미리 가져오기
	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	const FSkillResourceData* ResourceDataPtr = nullptr;

	if (DataSys && CachedResourceID != NAME_None)
	{
		ResourceDataPtr = &DataSys->GetSkillResourceData(CachedResourceID);
	}

	FGameplayTag HostileTag = GetHostileTeamTag();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerChar);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), StrikeCenter, CachedHitRadius, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);

#if WITH_EDITOR
	// 스킬이 터지는 곳에 빨간 범위 표시
	DrawDebugSphere(GetWorld(), StrikeCenter, CachedHitRadius, 16, FColor::Red, false, 0.5f);
#endif

	for (AActor* HitActor : OutActors)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (TargetASC && TargetASC->HasMatchingGameplayTag(HostileTag) && !TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
		{
			FGameplayEffectContextHandle Context = GetOwnerASC()->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = GetOwnerASC()->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

			if (SpecHandle.IsValid())
			{
				GetOwnerASC()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}

			// 데미지를 입혔을 때, ImpactVFX 함수 호출
			if (ResourceDataPtr)
			{
				SpawnImpactVFX(HitActor, *ResourceDataPtr);
			}
		}
	}

	// 스폰 이펙트 로직
	if (ResourceDataPtr)
	{
		FRotator RandomRot = FRotator::ZeroRotator;
		RandomRot.Yaw = FMath::RandRange(0.0f, 360.0f);
		RandomRot.Roll = FMath::RandRange(-80.0f, 80.0f);
		RandomRot.Pitch = FMath::RandRange(-45.0f, 45.0f);

		if (UNiagaraSystem* LoadedVFX = ResourceDataPtr->SpawnVFX.LoadSynchronous())
		{
			FVector Scale(4.0f, 4.0f, 4.0f);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedVFX, StrikeCenter, RandomRot, Scale);
		}

		if (USoundBase* LoadedSFX = ResourceDataPtr->SpawnSFX.LoadSynchronous())
		{
			UGameplayStatics::PlaySoundAtLocation(this, LoadedSFX, StrikeCenter);
		}
	}
}

void ULRGA_RapidSlash::SpawnImpactVFX(AActor* InTargetActor, const FSkillResourceData& InResourceData)
{
	if (!InTargetActor) return;

	if (UNiagaraSystem* LoadedVFX = InResourceData.ImpactVFX.LoadSynchronous())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedVFX, InTargetActor->GetActorLocation());
	}
}



void ULRGA_RapidSlash::OnStrikeMontageFinished()
{
	CurrentStrikeCount++;
	StartNextStrike();
}


