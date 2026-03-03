// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/LRHomingProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/GameInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"

void ALRHomingProjectile::OnSkillObjectInitialized()
{
	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("DataSubsystem Loading is Failed")))
	{
		return;
	}

	const FFlightHomingData& HomingData = DataSys->GetFlightData<FFlightHomingData>(InitData.SkillEffectID);

	// 적대 팀 태그 추출
	FGameplayTag HostileTag = GetHostileTeamTag();
	if (!HostileTag.IsValid())
	{
		LR_INFO(TEXT("유효하지 않은 HostileTag → Linear 전환"));
		return;
	}

	// LockRange 내 가장 가까운 적 탐색
	AActor* Target = FindNearestHostile(HostileTag, HomingData.LockRange);

	if (!Target)
	{
		LR_INFO(TEXT("범위 내 적 없음 → Linear 전환"));
		return; // bIsHomingProjectile 기본값 false → Linear 유지
	}

	// 유도 세팅
	ProjectileComp->bIsHomingProjectile = true;
	ProjectileComp->HomingAccelerationMagnitude = HomingData.TurnSpeed;
	ProjectileComp->HomingTargetComponent = Target->GetRootComponent();

	LR_INFO(TEXT("타겟 지정 성공: %s"), *Target->GetName());
}

void ALRHomingProjectile::OnPoolDeactivate_Implementation()
{
	// Homing 비활성화 — 풀 복귀 후 재사용 시 오작동 방지
	ProjectileComp->bIsHomingProjectile = false;
	ProjectileComp->HomingTargetComponent = nullptr;
	
	Super::OnPoolDeactivate_Implementation();
}

AActor* ALRHomingProjectile::FindNearestHostile(FGameplayTag HostileTag, float SearchRadius) const
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<ALRHomingProjectile*>(this));
	IgnoreActors.Add(Cast<AActor>(GetInstigator()));

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(), GetActorLocation(), SearchRadius,
		16, FColor::Green, false, 2.0f);
#endif

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), GetActorLocation(),
		SearchRadius, ObjectTypes,
		nullptr, IgnoreActors, OutActors);

	AActor* Nearest = nullptr;
	float MinDistSq = FLT_MAX;

	for (AActor* Candidate : OutActors)
	{
		UAbilitySystemComponent* CandidateASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Candidate);

		if (!CandidateASC)
		{
			continue;
		}

		// 적대 태그 체크
		if (!CandidateASC->HasMatchingGameplayTag(HostileTag))
		{
			continue;
		}

		// 사망 제외
		if (CandidateASC->HasMatchingGameplayTag(LRTags::State_Dead))
		{
			continue;
		}

		float DistSq = FVector::DistSquared(GetActorLocation(), Candidate->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Nearest = Candidate;
		}
	}

	return Nearest;
}
