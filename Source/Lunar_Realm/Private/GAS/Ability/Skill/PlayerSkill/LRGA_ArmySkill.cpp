// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_ArmySkill.h"
#include "DrawDebugHelpers.h"
#include "Units/LRCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


ULRGA_ArmySkill::ULRGA_ArmySkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Army;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_ARMY";
	SkillEffectID = "EFFECT_ARMY";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Army.Cooldown")));
}

void ULRGA_ArmySkill::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	LR_INFO(TEXT("[LRGA_Army] 스킬 발동!"));

	if (!CachedInstigator || !StrikeMontage || !DamageEffectClass)
	{
		LR_WARN(TEXT("[LRGA_Army] 필수 데이터(Montage 또는 GE)가 누락되었습니다."));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys)
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	CachedHitRadius = EffectData.Range > 0.f ? EffectData.Range : 300.0f;
	CachedResourceID = SkillData.ResourceID;

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, StrikeMontage, 1.0f);

	MontageTask->OnBlendOut.AddDynamic(this, &ULRGA_ArmySkill::OnStrikeMontageFinished);
	MontageTask->OnCompleted.AddDynamic(this, &ULRGA_ArmySkill::OnStrikeMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &ULRGA_ArmySkill::OnStrikeMontageFinished);
	MontageTask->OnCancelled.AddDynamic(this, &ULRGA_ArmySkill::OnStrikeMontageFinished);

	MontageTask->ReadyForActivation();

	PerformStrikeLogic();
}

void ULRGA_ArmySkill::PerformStrikeLogic()
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerChar) return;

	FVector StrikeCenter = OwnerChar->GetActorLocation() + (OwnerChar->GetActorForwardVector() * StrikeDistance);

	FGameplayTag HostileTag = GetHostileTeamTag();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerChar);

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), StrikeCenter, CachedHitRadius, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), StrikeCenter, CachedHitRadius, 16, FColor::Red, false, 0.5f);
#endif

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	const FSkillResourceData* ResourceData = (DataSys && CachedResourceID != NAME_None) ? &DataSys->GetSkillResourceData(CachedResourceID) : nullptr;

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

				if (ResourceData)
				{
					if (UNiagaraSystem* LoadedImpactVFX = ResourceData->ImpactVFX.LoadSynchronous())
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedImpactVFX, HitActor->GetActorLocation());
					}

					if (USoundBase* LoadedImpactSFX = ResourceData->ImpactSFX.LoadSynchronous())
					{
						UGameplayStatics::PlaySoundAtLocation(this, LoadedImpactSFX, HitActor->GetActorLocation());
					}
				}
			}
		}
	}

	if (ResourceData)
	{
		if (UNiagaraSystem* LoadedVFX = ResourceData->SpawnVFX.LoadSynchronous())
		{
			FRotator SpawnRot = OwnerChar->GetActorRotation();
			FVector Scale(1.0f, 1.0f, 1.0f);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedVFX, StrikeCenter, SpawnRot, Scale);
		}

		if (USoundBase* LoadedSFX = ResourceData->SpawnSFX.LoadSynchronous())
		{
			UGameplayStatics::PlaySoundAtLocation(this, LoadedSFX, StrikeCenter);
		}
	}
}

void ULRGA_ArmySkill::OnStrikeMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}