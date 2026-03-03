// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/LRExplodeProjectile.h"

#include "DrawDebugHelpers.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"

void ALRExplodeProjectile::OnSkillObjectInitialized()
{
	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("DataSubsystem Loading is Failed")))
	{
		return;
	}
	
	const FFlightExplodeData& ExplodeData = DataSys->GetFlightData<FFlightExplodeData>(InitData.SkillEffectID);
	
	CachedExplosionRadius = ExplodeData.ExplosionRadius;
	CachedExplosionDamageMultiplier = ExplodeData.ExplosionDamageMultiplier;
}

bool ALRExplodeProjectile::OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit)
{
	OnSkillObjectExpired();
	OnPoolDeactivate_Implementation();
	return false; //자식이 직접 풀 복귀시킴
}

void ALRExplodeProjectile::OnSkillObjectExpired()
{
	ApplyExplosionDamage();
}

void ALRExplodeProjectile::ApplyExplosionDamage()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(Cast<AActor>(GetInstigator()));
	
	// 디버그 구체 그리기 (에디터 빌드에서만)
#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(), CachedExplosionRadius,  
		16, FColor::Red, false, 2.0f );
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

	
	//데미지 처리
	for (AActor* target : OutActors)
	{
		//적대 태그가 없으면 무시
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(target);
		if (!TargetASC)
		{
			continue;
		}
		if (!TargetASC->HasMatchingGameplayTag(HostileTag))
		{
			continue;
		}
		
		float distance = FVector::Dist(GetActorLocation(), target->GetActorLocation());
		float fallOffRatio = 1.f - FMath::Clamp(distance / CachedExplosionRadius, 0.f, 1.f);
		float finalDamage = InitData.Damage * CachedExplosionDamageMultiplier * fallOffRatio;
		
		LR_INFO(TEXT("Explosion Damage to %s : %f"), *target->GetName(), finalDamage);
		
		ApplyEffectToTarget(target, InitData.DamageEffectClass, finalDamage);
		
		if (InitData.StatusEffectClass)
		{
			ApplyEffectToTarget(target, InitData.StatusEffectClass, 0.f);
		}
	}
}
