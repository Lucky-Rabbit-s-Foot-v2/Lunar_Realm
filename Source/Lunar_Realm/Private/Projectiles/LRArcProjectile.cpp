// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/LRArcProjectile.h"

#include "DrawDebugHelpers.h"
#include "Engine/GameInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"

void ALRArcProjectile::OnSkillObjectInitialized()
{
	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("DataSubsystem Loading is Failed")))
	{
		return;
	}

	const FFlightArcData& ArcData = DataSys->GetFlightData<FFlightArcData>(InitData.SkillEffectID);
	const FFlightExplodeData& ExplodeData = DataSys->GetFlightData<FFlightExplodeData>(InitData.SkillEffectID);

	// 폭발 파라미터 캐싱
	CachedExplosionRadius = ExplodeData.ExplosionRadius;
	CachedExplosionDamageMultiplier = ExplodeData.ExplosionDamageMultiplier;

	// 중력 세팅
	ProjectileComp->ProjectileGravityScale = ArcData.GravityScale;

	// 캐릭터 머리 위에서 발사되도록 Z 오프셋 적용
	FVector AdjustedLocation = GetActorLocation() + FVector(0.f, 0.f, 90.f);
	SetActorLocation(AdjustedLocation);
	
	// LaunchAngle 기반 초기 속도 벡터 재계산
	float AngleRadian = FMath::DegreesToRadians(ArcData.LaunchAngle);
	FVector LaunchDir = (GetActorForwardVector() * FMath::Cos(AngleRadian)) + (FVector::UpVector * FMath::Sin(AngleRadian));
	ProjectileComp->Velocity = LaunchDir * InitData.Speed;
}

bool ALRArcProjectile::OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit)
{
	OnSkillObjectExpired();
	OnPoolDeactivate_Implementation();
	return false; // 자식이 직접 풀 복귀
}

void ALRArcProjectile::OnSkillObjectExpired()
{
	ApplyExplosionDamage();
	// 풀 복귀는 베이스 OnLifeTimeExpired가 처리
}

void ALRArcProjectile::ApplyExplosionDamage()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(Cast<AActor>(GetInstigator()));

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(), CachedExplosionRadius,
		16, FColor::Cyan, false, 2.0f);
#endif

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), GetActorLocation(),
		CachedExplosionRadius, ObjectTypes,
		nullptr, IgnoreActors, OutActors);

	//적대 태그 검출
	FGameplayTag HostileTag = GetHostileTeamTag();
	if (!HostileTag.IsValid())
	{
		LR_WARN(TEXT("Invalid HostileTag "));
		return;
	}
	
	for (AActor* Target : OutActors)
	{
		//적대 태그가 없으면 무시
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC)
		{
			continue;
		}
		if (!TargetASC->HasMatchingGameplayTag(HostileTag))
		{
			continue;
		}
		
		float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
		float FallOffRatio = 1.f - FMath::Clamp(Distance / CachedExplosionRadius, 0.f, 1.f); //범위에서 가까운 순으로 감쇠데미지
		float FinalDamage = InitData.Damage * CachedExplosionDamageMultiplier * FallOffRatio;

		LR_INFO(TEXT("Arc Explosion Damage to %s : %f"), *Target->GetName(), FinalDamage);

		ApplyEffectToTarget(Target, InitData.DamageEffectClass, FinalDamage);

		if (InitData.StatusEffectClass)
		{
			ApplyEffectToTarget(Target, InitData.StatusEffectClass, 0.f);
		}
	}
}
