// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/LRGameplayAbilityBase.h"

#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Projectiles/LRProjectile.h"
#include "Units/LRCharacter.h"

ULRGameplayAbilityBase::ULRGameplayAbilityBase()
{
	//기본 인스턴스 정책은 액터별 처리
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//멀티플레이 어빌리티 실행 정책은 클라이언트 예측 실행 후 서버 확정 방식
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void ULRGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData)
	{
		CachedInstigator = Cast<ALRCharacter>(TriggerEventData->Instigator.Get());
		CachedTarget = TriggerEventData->Target.Get();
	}
	
	OnAbilityActivated(Handle, ActorInfo, ActivationInfo);
}

void ULRGameplayAbilityBase::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	//자식 GA들의 실질적인 로직 구성.(Super 필요없음)
}

void ULRGameplayAbilityBase::SpawnProjectiles(TSubclassOf<ALRProjectile> ProjectileClass,
	const FSkillObjectInitData& InitData)
{
	if (!CachedInstigator || !ProjectileClass)
    {
        LR_WARN(TEXT("[SpawnProjectiles] 유효하지 않은 Instigator 또는 ProjectileClass"));
        return;
    }

    const FSkillSpawnData& SpawnData = InitData.SpawnData;
    int32 Count = FMath::Max(1, SpawnData.ProjectileCount);

    // ProjectileCount가 1인데 FanSpread면 Single로 처리
    if (Count == 1 && SpawnData.SpawnPattern == ESpawnPattern::SPREAD)
    {
        LR_WARN(TEXT("[SpawnProjectiles] ProjectileCount=1인데 FanSpread → Single로 처리"));
    }

    for (int32 i = 0; i < Count; i++)
    {
        // 스폰 위치 — SocketName 유효하지 않으면 전방 200cm
        FVector SpawnLocation = CachedInstigator->GetActorLocation()
            + CachedInstigator->GetActorForwardVector() * 200.f;
        FRotator SpawnRotation = CachedInstigator->GetActorRotation();

        // SpawnPattern별 방향 계산
        switch (SpawnData.SpawnPattern)
        {
        case ESpawnPattern::SPREAD:
            if (Count > 1)
            {
                // 부채꼴 균등 분배
                // ex) Count=3, SpreadAngle=60 → -30, 0, +30
                float HalfAngle = SpawnData.SpreadAngle * 0.5f;
                float Step = SpawnData.SpreadAngle / (Count - 1);
                float Yaw = -HalfAngle + Step * i;
                SpawnRotation.Yaw += Yaw;
            }
            break;

        case ESpawnPattern::CIRCLE:
            {
                // 360도 균등 분배
                float Yaw = (360.f / Count) * i;
                SpawnRotation.Yaw += Yaw;
            }
            break;

        case ESpawnPattern::SINGLE:
        default:
            break; // 방향 변경 없음
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner      = const_cast<ALRCharacter*>(CachedInstigator.Get());
        SpawnParams.Instigator = const_cast<ALRCharacter*>(CachedInstigator.Get());

        ALRProjectile* Projectile = GetWorld()->SpawnActor<ALRProjectile>(
            ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (!Projectile)
        {
            LR_WARN(TEXT("[SpawnProjectiles] 투사체 스폰 실패"));
            continue;
        }

        Projectile->InitSkillObject(InitData);
    }
}

void ULRGameplayAbilityBase::SpawnProjectiles(TSubclassOf<ALRProjectile> ProjectileClass,
	const FSkillObjectInitData& InitData, FRotator BaseRotation)
{
	if (!CachedInstigator || !ProjectileClass)
    {
        LR_WARN(TEXT("[SpawnProjectiles] 유효하지 않은 Instigator 또는 ProjectileClass"));
        return;
    }

    const FSkillSpawnData& SpawnData = InitData.SpawnData;
    int32 Count = FMath::Max(1, SpawnData.ProjectileCount);

    // ProjectileCount가 1인데 FanSpread면 Single로 처리
    if (Count == 1 && SpawnData.SpawnPattern == ESpawnPattern::SPREAD)
    {
        LR_WARN(TEXT("[SpawnProjectiles] ProjectileCount=1인데 FanSpread → Single로 처리"));
    }

    for (int32 i = 0; i < Count; i++)
    {
        // 스폰 위치 — SocketName 유효하지 않으면 전방 200cm
        FVector SpawnLocation = CachedInstigator->GetActorLocation()
            + CachedInstigator->GetActorForwardVector() * 200.f;
        FRotator SpawnRotation = BaseRotation;

        // SpawnPattern별 방향 계산
        switch (SpawnData.SpawnPattern)
        {
        case ESpawnPattern::SPREAD:
            if (Count > 1)
            {
                // 부채꼴 균등 분배
                // ex) Count=3, SpreadAngle=60 → -30, 0, +30
                float HalfAngle = SpawnData.SpreadAngle * 0.5f;
                float Step = SpawnData.SpreadAngle / (Count - 1);
                float Yaw = -HalfAngle + Step * i;
                SpawnRotation.Yaw += Yaw;
            }
            break;

        case ESpawnPattern::CIRCLE:
            {
                // 360도 균등 분배
                float Yaw = (360.f / Count) * i;
                SpawnRotation.Yaw += Yaw;
            }
            break;

        case ESpawnPattern::SINGLE:
        default:
            break; // 방향 변경 없음
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner      = const_cast<ALRCharacter*>(CachedInstigator.Get());
        SpawnParams.Instigator = const_cast<ALRCharacter*>(CachedInstigator.Get());

        ALRProjectile* Projectile = GetWorld()->SpawnActor<ALRProjectile>(
            ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (!Projectile)
        {
            LR_WARN(TEXT("[SpawnProjectiles] 투사체 스폰 실패"));
            continue;
        }

        Projectile->InitSkillObject(InitData);
    }
}

ALRCharacter* ULRGameplayAbilityBase::GetCharacterFromActorInfo(const FGameplayAbilityActorInfo& ActorInfo) const
{
	return Cast<ALRCharacter>(ActorInfo.AvatarActor.Get());
}


UAbilitySystemComponent* ULRGameplayAbilityBase::GetOwnerASC() const
{
	return GetAbilitySystemComponentFromActorInfo_Ensured();
}

FGameplayTag ULRGameplayAbilityBase::GetHostileTeamTag() const
{
	if (!CachedInstigator)
	{
		LR_WARN(TEXT("유효하지 않은 Instigator"));
		return FGameplayTag::EmptyTag;
	}
	
	UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(	const_cast<ALRCharacter*>(CachedInstigator.Get()));

	if (!InstigatorASC)
	{
		LR_WARN(TEXT("유효하지 않은 ASC"));
		return FGameplayTag::EmptyTag;
	}
	
	//ASC의 Instigator타입에 따라 팀 태그 반환
	if (InstigatorASC->HasMatchingGameplayTag(LRTags::Team_Player))
	{
		return LRTags::Team_Enemy;
	}
	
	if (InstigatorASC->HasMatchingGameplayTag(LRTags::Team_Enemy))
	{
		return LRTags::Team_Player;
	}
	
	return FGameplayTag::EmptyTag;
}
