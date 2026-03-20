// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRGA_BasicAttack.h"
#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "Units/Player/LRPlayerState.h"
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
#include "Subsystems/GameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"


ULRGA_BasicAttack::ULRGA_BasicAttack()
{
	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.AddTag(LRTags::Ability_Combat_BasicShoot);
	SetAssetTags(TempTags);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//CooldownTagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.BasicAttack")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.BasicAttack")));

	
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
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(FName("Cooldown.Skill.BasicAttack"));

	if (!GetCooldownGameplayEffect())
	{
		LR_ERROR(TEXT("Cooldown GE가 비어있음"));
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		LR_ERROR(TEXT("[GA_Attack] 평타 GA 실행 실패 (CommitAbility 실패)"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(CooldownGE->GetClass(), 1.0f, ASC->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(CooldownTag);

			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	if (ASC && ASC->GetNumericAttributeBase(ULRAttributeSet::GetHealthAttribute()) <= 0.0f)
	{
		LR_WARN(TEXT("[GA_Attack] 사망한 상태 공격 강제 취소"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 디버그 로그
	//LR_WARN(TEXT("======================================"));
	//LR_WARN(TEXT("[디버그] 1. 현재 실행 중인 스킬 클래스: %s"), *GetClass()->GetName());

	//if (GetCooldownGameplayEffect())
	//{
	//	LR_WARN(TEXT("[디버그] 2. 쿨타임 GE: 정상적으로 들어있음"));
	//}
	//else
	//{
	//	LR_ERROR(TEXT("[디버그] 2. 쿨타임 GE: 텅 비어있음"));
	//}

	//LR_WARN(TEXT("[디버그] 3. Commit 전 쿨타임 태그 보유 여부: %d"), ASC->HasMatchingGameplayTag(CooldownTag));

	//if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	//{
	//	LR_ERROR(TEXT("[GA_Attack] 평타 GA 실행 실패 (CommitAbility 실패)"));
	//	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	//	return;
	//}

	//LR_WARN(TEXT("[디버그] 4. Commit 후 쿨타임 태그 보유 여부: %d"), ASC->HasMatchingGameplayTag(CooldownTag));
	//LR_WARN(TEXT("======================================"));

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

	PlayAttackGruntSound(OwnerChar);

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

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;

	// 근접 공격 (노티파이 데미지)
	if (bIsMeleeAttack)
	{
		LR_INFO(TEXT("[GA_Attack] 근접 공격 발동"));

		FGameplayEffectContextHandle Context = GetOwnerASC()->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = GetOwnerASC()->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

		if (SpecHandle.IsValid())
		{
			if (DataSys && SkillEffectID != NAME_None)
			{
				const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
			}
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
		if (!OwnerChar || !DataSys) return;

		const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
		const FSkillStaticData& StaticData = DataSys->GetSkillStaticData(SkillID);
		const FSkillSpawnData& SpawnData = DataSys->GetSkillSpawnData(SkillEffectID);

		FVector SpawnLocation = OwnerChar->GetActorLocation() + (OwnerChar->GetActorForwardVector() * 100.0f);
		FRotator FireRotation = OwnerChar->GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerChar;
		SpawnParams.Instigator = OwnerChar;

		ALRProjectile* SpawnedProj = GetWorld()->SpawnActor<ALRProjectile>(ProjectileClass, SpawnLocation, FireRotation, SpawnParams);

		if (SpawnedProj)
		{
			FSkillObjectInitData InitData;
			InitData.DamageEffectClass = DamageEffectClass;
			InitData.InstigatorASC = GetOwnerASC();
			InitData.SkillEffectID = SkillEffectID;
			InitData.ResourceID = StaticData.ResourceID;
			InitData.Damage = EffectData.Amount;
			InitData.Speed = EffectData.Speed;
			InitData.Lifetime = EffectData.Lifetime;
			InitData.SpawnData = SpawnData;

			SpawnedProj->InitSkillObject(InitData);

			// 유도(Homing) 기능 세팅
			if (TargetActor)
			{
				UProjectileMovementComponent* ProjMovement = SpawnedProj->FindComponentByClass<UProjectileMovementComponent>();
				if (ProjMovement && ProjMovement->bIsHomingProjectile)
				{
					ProjMovement->HomingTargetComponent = TargetActor->GetRootComponent();
				}
			}
		}
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

void ULRGA_BasicAttack::PlayAttackGruntSound(ALRCharacter* InOwnerChar)
{
	if (!InOwnerChar) return;

	ALRPlayerState* PS = InOwnerChar->GetPlayerState<ALRPlayerState>();
	if (!PS) return;

	FName CharID = PS->GetCharacterID();

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys) return;

	const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(CharID);

	if (CharData.CharacterName.IsEmpty())
	{
		LR_WARN(TEXT("[GA_Attack] CharacterName이 비어있어서 사운드를 찾을 수 없음"));
		return;
	}

	FName CharName = FName(*CharData.CharacterName);
	const FCharacterSoundData& SoundData = DataSys->GetCharacterSoundData(CharName);

	if (SoundData.AttackGrunts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, SoundData.AttackGrunts.Num() - 1);

		if (USoundBase* LoadedSound = SoundData.AttackGrunts[RandomIndex].LoadSynchronous())
		{
			UGameplayStatics::PlaySoundAtLocation(InOwnerChar, LoadedSound, InOwnerChar->GetActorLocation());
		}
	}
}
