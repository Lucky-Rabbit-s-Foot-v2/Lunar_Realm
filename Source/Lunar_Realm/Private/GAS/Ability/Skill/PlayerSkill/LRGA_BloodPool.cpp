// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_BloodPool.h"
#include "Structures/SkillActor/LRAoEActor.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Units/LRCharacter.h"
#include "Subsystems/GameDataSubsystem.h"
#include "System/LoggingSystem.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"

ULRGA_BloodPool::ULRGA_BloodPool()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_BloodPool;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_BLOODPOOL";
	SkillEffectID = "EFFECT_BLOODPOOL";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.BloodPool.Cooldown")));
}

void ULRGA_BloodPool::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	LR_INFO(TEXT("[BloodPool/Anubis] 스킬 발동 시작!"));
		
	if (!CachedInstigator.IsValid())
	{
		LR_WARN(TEXT("에러: CachedInstigator가 없습니다!"));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	if (!AoEActorClass)
	{
		LR_WARN(TEXT("에러: 블루프린트에 AoEActorClass가 안 들어있습니다!"));
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	FGameplayEventData DummyPayload;
	OnHitEventReceived(DummyPayload);

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}

void ULRGA_BloodPool::OnHitEventReceived(FGameplayEventData InPayload)
{
	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys || !CachedInstigator.IsValid()) return;

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	// 기획 수치 세팅
	float SearchRadius = 1000.0f;           // 탐색 범위
	int32 MaxPoolCount = 5;                 // 최대 소환 개수
	float MinDistanceBetweenPools = 250.0f; // 웅덩이 간 최소 간격

	FGameplayTag HostileTag = GetHostileTeamTag();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<ALRCharacter*>(CachedInstigator.Get()));

	// 1000 범위 안의 모든 대상 찾기
	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CachedInstigator->GetActorLocation(), SearchRadius, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);

	// 선택된 스폰 위치들을 저장할 배열
	TArray<FVector> SelectedLocations;

	// 적들을 순회하며 겹치지 않는 위치 선정
	for (AActor* TargetActor : OutActors)
	{
		if (SelectedLocations.Num() >= MaxPoolCount) break;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC && TargetASC->HasMatchingGameplayTag(HostileTag) && !TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
		{
			FVector TargetLoc = TargetActor->GetActorLocation();
			bool bIsTooClose = false;

			for (const FVector& SelectedLoc : SelectedLocations)
			{
				if (FVector::Dist(TargetLoc, SelectedLoc) < MinDistanceBetweenPools)
				{
					bIsTooClose = true;
					break;
				}
			}

			if (!bIsTooClose)
			{
				SelectedLocations.Add(TargetLoc);
			}
		}
	}

	if (SelectedLocations.IsEmpty())
	{
		FVector DefaultLoc = CachedInstigator->GetActorLocation() + (CachedInstigator->GetActorForwardVector() * 300.0f);
		SelectedLocations.Add(DefaultLoc);
	}

	for (FVector SpawnLoc : SelectedLocations)
	{
		SpawnLoc.Z -= 88.0f;
		SpawnSinglePool(SpawnLoc, EffectData, SkillData);
	}

	


	//UGameInstance* GI = GetWorld()->GetGameInstance();
	//UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	//if (!DataSys) return;

	//const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	//const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	//FVector SpawnLocation = CachedInstigator->GetActorLocation();

	//SpawnLocation.Z -= 88.0f;

	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = const_cast<ALRCharacter*>(CachedInstigator.Get());
	//SpawnParams.Instigator = const_cast<ALRCharacter*>(CachedInstigator.Get());

	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//ALRAoEActor* SpawnedAoE = GetWorld()->SpawnActor<ALRAoEActor>(AoEActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	//if (SpawnedAoE)
	//{
	//	LR_INFO(TEXT("[BloodPool/Anubis] 장판 스폰 완벽 성공!"));
	//	float Radius = EffectData.Range > 0.f ? EffectData.Range : 400.0f;
	//	float Duration = EffectData.Lifetime > 0.f ? EffectData.Lifetime : 5.0f;

	//	SpawnedAoE->InitAoE(
	//		GetOwnerASC(),
	//		DamageEffectClass,
	//		GetHostileTeamTag(),
	//		SkillData.ResourceID,
	//		Radius,
	//		Duration
	//	);
	//}
	//else
	//{
	//	LR_WARN(TEXT("에러: 장판 스폰에 실패했습니다! (충돌 문제일 가능성 높음)"));
	//}
}

void ULRGA_BloodPool::SpawnSinglePool(FVector InSpawnLocation, const FSkillEffectData& InEffectData, const FSkillStaticData& InSkillData)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = const_cast<ALRCharacter*>(CachedInstigator.Get());
	SpawnParams.Instigator = const_cast<ALRCharacter*>(CachedInstigator.Get());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALRAoEActor* SpawnedAoE = GetWorld()->SpawnActor<ALRAoEActor>(AoEActorClass, InSpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (SpawnedAoE)
	{
		float Radius = InEffectData.Range > 0.f ? InEffectData.Range : 400.0f;
		float Duration = InEffectData.Lifetime > 0.f ? InEffectData.Lifetime : 5.0f;

		SpawnedAoE->InitAoE(
			GetOwnerASC(),
			DamageEffectClass,
			GetHostileTeamTag(),
			InSkillData.ResourceID,
			Radius,
			Duration
		);
	}
}
