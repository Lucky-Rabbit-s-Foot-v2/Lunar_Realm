// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/PlayerSkill/LRGA_HomingFairyArrow.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Units/LRCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"

ULRGA_HomingFairyArrow::ULRGA_HomingFairyArrow()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_HomingFairyArrow;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	SkillID = "SKILL_HOMINGFAIRY";
	SkillEffectID = "EFFECT_HOMINGFAIRY";

	CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.HomingFairyArrow.Cooldown")));
}

void ULRGA_HomingFairyArrow::OnAbilityActivated(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo)
{
	if (!CachedInstigator || !ProjectileClass || !DamageEffectClass)
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
	const FSkillSpawnData& SpawnData = DataSys->GetSkillSpawnData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);

	FSkillObjectInitData InitData;
	InitData.DamageEffectClass = DamageEffectClass;
	InitData.InstigatorASC = GetOwnerASC();
	InitData.ResourceID = SkillData.ResourceID;
	InitData.SkillEffectID = SkillEffectID;
	InitData.Damage = EffectData.Amount;
	InitData.Speed = EffectData.Speed;
	InitData.Lifetime = EffectData.Lifetime;
	InitData.SpawnData = SpawnData;

	SpawnDistributedArrows(ProjectileClass, InitData);

	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}

void ULRGA_HomingFairyArrow::SpawnDistributedArrows(TSubclassOf<ALRProjectile> InProjectileClass, const FSkillObjectInitData& InInitData)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<ALRCharacter*>(CachedInstigator.Get()));

	TArray<AActor*> OutActors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), CachedInstigator->GetActorLocation(), 1500.0f, ObjectTypes, AActor::StaticClass(), IgnoreActors, OutActors);

	TArray<AActor*> ValidEnemies;
	FGameplayTag HostileTag = GetHostileTeamTag();

	for (AActor* Enemy : OutActors)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy);
		if (TargetASC && TargetASC->HasMatchingGameplayTag(HostileTag) && !TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
		{
			ValidEnemies.Add(Enemy);
		}
	}

	// 화살 발사 개수 및 기준 각도 계산
	int32 Count = FMath::Max(1, InInitData.SpawnData.ProjectileCount);
	FRotator BaseRotation = CachedInstigator->GetActorRotation();

	for (int32 i = 0; i < Count; i++)
	{
		FSkillObjectInitData ArrowData = InInitData;

		// 360도 원형으로 쏘기 위한 각도 계산
		FVector SpawnLocation = CachedInstigator->GetActorLocation() + CachedInstigator->GetActorForwardVector() * 50.f;
		FRotator SpawnRotation = BaseRotation;

		if (InInitData.SpawnData.SpawnPattern == ESpawnPattern::CIRCLE)
		{
			float Yaw = (360.f / Count) * i;
			SpawnRotation.Yaw += Yaw;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = const_cast<ALRCharacter*>(CachedInstigator.Get());
		SpawnParams.Instigator = const_cast<ALRCharacter*>(CachedInstigator.Get());

		ALRProjectile* Projectile = GetWorld()->SpawnActor<ALRProjectile>(InProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (Projectile)
		{

			Projectile->InitSkillObject(ArrowData);

			// 즉시 켜진 유도를 강제로 끄고, 0.3초 뒤에 켜지도록 타이머 예약
			if (ValidEnemies.Num() > 0)
			{
				AActor* AssignedTarget = ValidEnemies[i % ValidEnemies.Num()];

				if (UProjectileMovementComponent* MovementComp = Projectile->FindComponentByClass<UProjectileMovementComponent>())
				{
					MovementComp->bIsHomingProjectile = false;
					MovementComp->HomingTargetComponent = nullptr;

					// 포인터로 변환 
					TWeakObjectPtr<UProjectileMovementComponent> WeakMovement = MovementComp;
					TWeakObjectPtr<AActor> WeakTarget = AssignedTarget;

					// 0.3초 뒤에 실행될 예약 함수 만들기
					FTimerHandle HomingTimer;
					FTimerDelegate HomingDelegate = FTimerDelegate::CreateLambda([WeakMovement, WeakTarget]()
						{
							if (WeakMovement.IsValid() && WeakTarget.IsValid())
							{
								// 0.3초 뒤에 타겟을 물고 유도 모드
								WeakMovement->bIsHomingProjectile = true;
								WeakMovement->HomingTargetComponent = WeakTarget->GetRootComponent();
								WeakMovement->HomingAccelerationMagnitude = 4000.f;
							}
						});

					// 타이머 실행
					GetWorld()->GetTimerManager().SetTimer(HomingTimer, HomingDelegate, 0.3f, false);
				}
			}
		}
	}

}
