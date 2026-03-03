// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/LRPierceProjectile.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

void ALRPierceProjectile::OnSkillObjectInitialized()
{
	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("DataSubsystem Loading is Failed")))
	{
		return;
	}

	const FFlightPierceData& PierceData = DataSys->GetFlightData<FFlightPierceData>(InitData.SkillEffectID);

	CachedPierceCount = PierceData.PierceCount;
	CachedDamageDecay = PierceData.DamageDecay;
	CachedCurrentDamage = InitData.Damage;
	HitActors.Empty();
}

bool ALRPierceProjectile::OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit)
{
	// 이미 충돌한 대상 무시
	if (HitActors.Contains(OtherActor))
	{
		return false;
	}
	HitActors.Add(OtherActor);

	// 현재 데미지로 GE 적용
	ApplyEffectToTarget(OtherActor, InitData.DamageEffectClass, CachedCurrentDamage);

	if (InitData.StatusEffectClass)
	{
		ApplyEffectToTarget(OtherActor, InitData.StatusEffectClass, 0.f);
	}

	// 관통 카운트 차감
	CachedPierceCount--;

	if (CachedPierceCount <= 0)
	{
		// 관통 소진 → 풀 복귀
		OnPoolDeactivate_Implementation();
	}
	else
	{
		// 다음 타격을 위해 데미지 감쇠
		CachedCurrentDamage *= CachedDamageDecay;
	}

	return false; // 자식이 직접 처리
}

void ALRPierceProjectile::OnPoolDeactivate_Implementation()
{
	HitActors.Empty();
	Super::OnPoolDeactivate_Implementation();
}
