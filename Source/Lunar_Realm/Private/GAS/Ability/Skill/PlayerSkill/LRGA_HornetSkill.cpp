// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_HornetSkill.h"
#include "DrawDebugHelpers.h"
#include "Units/LRCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"


ULRGA_HornetSkill::ULRGA_HornetSkill()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Hornet;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_HORNET";
	SkillEffectID = "EFFECT_HORNET";

	CooldownTagContainer.Reset();
	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Hornet.Cooldown")));

	// 스킬중에 평타 안나가게 막기
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot")));
}

void ULRGA_HornetSkill::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	LR_INFO(TEXT("[LRGA_HornetSkill] 에너지파 발사"));

	if (!CachedInstigator.IsValid() || !DamageEffectClass)
	{
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

	float Duration = EffectData.Lifetime > 0.f ? EffectData.Lifetime : 5.0f;
	BeamLength = EffectData.Range > 0.f ? EffectData.Range : 1500.0f;

	if (SkillData.ResourceID != NAME_None)
	{
		const FSkillResourceData& ResourceData = DataSys->GetSkillResourceData(SkillData.ResourceID);
		if (UNiagaraSystem* LoadedVFX = ResourceData.SpawnVFX.LoadSynchronous())
		{
			SpawnedBeamVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
				LoadedVFX,
				CachedInstigator->GetRootComponent(),
				NAME_None,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true
			);
		}

		if (USoundBase* LoadedSFX = ResourceData.SpawnSFX.LoadSynchronous())
		{
			UGameplayStatics::PlaySoundAtLocation(this, LoadedSFX, CachedInstigator->GetActorLocation());
		}
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DamageTimerHandle, this, &ULRGA_HornetSkill::TickDamage, TickRate, true);
		World->GetTimerManager().SetTimer(DurationTimerHandle, this, &ULRGA_HornetSkill::FinishSkill, Duration, false);
	}

	TickDamage();
}

void ULRGA_HornetSkill::TickDamage()
{
	if (!CachedInstigator.IsValid()) return;

	FVector StartLoc = CachedInstigator->GetActorLocation();
	FVector ForwardDir = CachedInstigator->GetActorForwardVector();

	FVector BoxCenter = StartLoc + (ForwardDir * (BeamLength * 0.5f));

	FVector BoxExtent(BeamLength * 0.5f, BeamRadius, BeamRadius);

	FRotator BoxRot = CachedInstigator->GetActorRotation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<ALRCharacter*>(CachedInstigator.Get()));

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), BoxCenter, BoxExtent, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);

#if WITH_EDITOR
	DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, BoxRot.Quaternion(), FColor::Red, false, 0.2f);
#endif

	FGameplayTag HostileTag = GetHostileTeamTag();

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	const FSkillStaticData* SkillData = DataSys ? &DataSys->GetSkillStaticData(SkillID) : nullptr;
	const FSkillResourceData* ResourceData = (DataSys && SkillData && SkillData->ResourceID != NAME_None) ? &DataSys->GetSkillResourceData(SkillData->ResourceID) : nullptr;

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
}

void ULRGA_HornetSkill::FinishSkill()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void ULRGA_HornetSkill::EndAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, bool bInReplicateEndAbility, bool bInWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DamageTimerHandle);
		World->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	if (SpawnedBeamVFX)
	{
		SpawnedBeamVFX->Deactivate();
		SpawnedBeamVFX = nullptr;
	}

	LR_INFO(TEXT("[LRGA_HornetSkill] 에너지파 발사 종료"));

	Super::EndAbility(InHandle, InActorInfo, InActivationInfo, bInReplicateEndAbility, bInWasCancelled);
}
