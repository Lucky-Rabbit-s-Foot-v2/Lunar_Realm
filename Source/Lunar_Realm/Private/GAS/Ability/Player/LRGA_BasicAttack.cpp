// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_BasicAttack.h"
#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "System/LoggingSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/LRProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


ULRGA_BasicAttack::ULRGA_BasicAttack()
{
	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Combat_BasicShoot);
	SetAssetTags(TempTags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	//(260219) KHS 이벤트 태그를 전달하여 발동되도록 트리거 등록
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Combat_BasicShoot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}


void ULRGA_BasicAttack::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	//UE_LOG(LogTemp, Warning, TEXT("[GA_Attack] 평타 GA 실행됨! 진입 성공!"));

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		LR_ERROR(TEXT("[GA_Attack] 평타 GA 실행 실패 (CommitAbility 실패)"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ASC && ASC->GetNumericAttributeBase(ULRAttributeSet::GetHealthAttribute()) <= 0.0f)
	{
		LR_WARN(TEXT("[GA_Attack] 사망한 상태 공격 강제 취소"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 공격자 정보
	ALRCharacter* OwnerChar = GetCharacterFromActorInfo(*ActorInfo);
	if (!OwnerChar)
	{
		LR_ERROR(TEXT("[GA_Attack] 실패: OwnerChar가 NULL임"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}


	// 타겟 정보
	const AActor* TargetActor = CachedTarget;
	if (!TargetActor)
	{
		LR_ERROR(TEXT("[GA_Attack] 실패: CachedTarget이 NULL임"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (!CachedTarget)
	{
		LR_ERROR(TEXT("[GA_Attack] CachedTarget이 NULL입니다! 캐스팅 실패!"));
	}

	// 적 공격시 타겟 방향으로 회전
	FVector StartLoc = OwnerChar->GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();

	TargetLoc.Z = StartLoc.Z;

	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(StartLoc, TargetLoc);

	OwnerChar->SetActorRotation(LookAtRot);


	// 몽타주 적용
	if (AttackMontage)
	{
		//OwnerChar->PlayAnimMontage(AttackMontage);

		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);

		MontageTask->OnBlendOut.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);
		MontageTask->OnCompleted.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &ULRGA_BasicAttack::OnMontageEnded);

		MontageTask->ReadyForActivation();

	}

	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, LRTags::Notify_Combat_BasicHit);
	EventTask->EventReceived.AddDynamic(this, &ULRGA_BasicAttack::OnHitEventReceived);
	EventTask->ReadyForActivation();
}

void ULRGA_BasicAttack::OnHitEventReceived(FGameplayEventData InPayload)
{
	const AActor* TargetActor = CachedTarget;
	if (!TargetActor) return;

	// 근접 공격 (노티파이 데미지)
	if (bIsMeleeAttack)
	{
		LR_INFO(TEXT("[GA_Attack] 근접 공격 발동"));

		FGameplayEffectContextHandle Context = GetOwnerASC()->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = GetOwnerASC()->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			if (TargetASC)
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
	// 원거리 공격 (투사체 발사)
	else if (ProjectileClass)
	{

		ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetAvatarActorFromActorInfo());
		if (!OwnerChar) return;

		FVector SpawnLocation = OwnerChar->GetActorLocation() + (OwnerChar->GetActorForwardVector() * 100.0f);
		FRotator FireRotation = OwnerChar->GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerChar;
		SpawnParams.Instigator = OwnerChar;

		// 1. 부모 함수 안 쓰고, 여기서 투사체를 직접 스폰!
		ALRProjectile* SpawnedProj = GetWorld()->SpawnActor<ALRProjectile>(ProjectileClass, SpawnLocation, FireRotation, SpawnParams);

		if (SpawnedProj)
		{
			// 2. 초기 데이터 세팅
			FSkillObjectInitData InitData;
			InitData.DamageEffectClass = DamageEffectClass;
			InitData.InstigatorASC = GetOwnerASC();
			InitData.Speed = ProjectileSpeed;
			InitData.Lifetime = 3.0f;

			SpawnedProj->InitSkillObject(InitData);

			// ★ 3. 핵심: 투사체한테 '쫓아갈 놈(Target)'의 위치(RootComponent)를 쥐여줌!
			if (TargetActor)
			{
				UProjectileMovementComponent* ProjMovement = SpawnedProj->FindComponentByClass<UProjectileMovementComponent>();

				// 블루프린트에서 '유도(Homing)' 옵션을 켰다면 타겟을 입력해준다!
				if (ProjMovement && ProjMovement->bIsHomingProjectile)
				{
					ProjMovement->HomingTargetComponent = TargetActor->GetRootComponent();
				}
			}
		}

		/*LR_INFO(TEXT("[GA_Attack] 원거리 투사체 발사 성공 투사체: %s"), *ProjectileClass->GetName());

		FSkillObjectInitData InitData;
		InitData.DamageEffectClass = DamageEffectClass;
		InitData.InstigatorASC = GetOwnerASC();
		InitData.Speed = ProjectileSpeed;
		InitData.Lifetime = 3.0f; 

		InitData.SpawnData.ProjectileCount = 1;
		InitData.SpawnData.SpawnPattern = ESpawnPattern::SINGLE;

		ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetAvatarActorFromActorInfo());
		FRotator FireRotation = OwnerChar ? OwnerChar->GetActorRotation() : FRotator::ZeroRotator;

		SpawnProjectiles(ProjectileClass, InitData, FireRotation);*/
	}
	else
	{
		LR_WARN(TEXT("[GA_Attack] 원거리 모드인데 ProjectileClass가 비어있음"));
	}
}

void ULRGA_BasicAttack::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	//LR_WARN(TEXT("몽타주 종료됨 스킬 끝"));
}