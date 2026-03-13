// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "Units/Player/LRPlayerState.h"
#include "Units/Player/Component/LRSummonComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameplayTagsManager.h"
#include "Data/LRDataStructs.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h" 
#include "Engine/World.h"
#include "Subsystems/GameDataSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/PrimitiveComponent.h"


ULRCombatComponent::ULRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SearchRadius = 1000.0f;
	AttackRange = 150.0f;
}

void ULRCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> AllBases;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy.Structure.Core"), AllBases);

	if (AllBases.Num() > 0)
	{
		CachedEnemyBase = AllBases[0];
		LR_INFO(TEXT("[Combat] 적 기지 캐싱 완료: %s"), *CachedEnemyBase->GetName());
	}
	else
	{
		LR_WARN(TEXT("[Combat] 맵에 'Enemy.Structure.Core' 태그를 가진 액터가 없음."));
	}

	float RandomDelay = FMath::RandRange(0.1f, 0.3f);
	GetWorld()->GetTimerManager().SetTimer(
		CombatLogicTimerHandle,
		this,
		&ULRCombatComponent::OnCombatLogicTimer,
		0.2f,
		true,
		RandomDelay
	);
}

void ULRCombatComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);
	
	// 오토모드시 스킬 자연스럽게 딜레이 넣음
	if (AutoSkillDelay > 0.0f)
	{
		AutoSkillDelay -= InDeltaTime;
	}
	
	// 기본공격 쿨다운
	if (CurrentAttackCooldown > 0.0f)
	{
		CurrentAttackCooldown -= InDeltaTime;
	}

	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter) return;

	UpdateTargetIndicator(OwnerCharacter);

	if (CurrentTarget)
	{
		if (CurrentTarget->GetActorLocation().Z < -2000.0f)
		{
			return;
		}
		ProcessCombatLogic(OwnerCharacter, InDeltaTime);
	}
}


// ============================================================================
// 퍼블릭 인터페이스
// ============================================================================

void ULRCombatComponent::SetAutoMode(bool bInEnableAuto)
{
	CombatState = bInEnableAuto ? EAutoCombatState::Auto : EAutoCombatState::Manual;
	LR_INFO(TEXT("전투모드 변경: %s"), bInEnableAuto ? TEXT("AUTO") : TEXT("MANUAL"));

	if (!bInEnableAuto)
	{
		ALRCharacter* OwnerCharacter = GetOwnerCharacter();
		if (OwnerCharacter && OwnerCharacter->GetController())
		{
			OwnerCharacter->GetController()->StopMovement();
		}
	}
}

void ULRCombatComponent::UpdateWeaponInfo(FName InWeaponID)
{
	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return;

	const FEquipmentStaticData& EquipData = DataSys->GetEquipmentStaticData(InWeaponID);
	ELRItemType ItemType = EquipData.ItemType;

	if (ItemType == ELRItemType::MELEE) AttackRange = 200.0f;
	else if (ItemType == ELRItemType::RANGED) AttackRange = 800.0f;
	else AttackRange = 100.0f;

	LR_INFO(TEXT("무기설정 ID: %s, Range: %.1f"), *InWeaponID.ToString(), AttackRange);
}


// ============================================================================
// 전투 메인 로직 
// ============================================================================

void ULRCombatComponent::OnCombatLogicTimer()
{
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter || IsTargetDead(OwnerCharacter))
	{
		if (CurrentTarget != nullptr)
		{
			ClearTarget();
		}
		return;
	}

	CheckAndClearDeadTarget();

	bool bIsManualMode = (CombatState == EAutoCombatState::Manual);
	bool bIsTargetingCore = (CurrentTarget == CachedEnemyBase);

	if (!CurrentTarget || bIsManualMode || bIsTargetingCore)
	{
		FindBestTarget();
	}
}

void ULRCombatComponent::ProcessCombatLogic(ALRCharacter* InOwnerCharacter, float InDeltaTime)
{
	bool bInBasicAttackRange = IsTargetInRange();
	AController* OwnerController = InOwnerCharacter->GetController();

	// 자동 소환 (독립 실행)
	if (CombatState == EAutoCombatState::Auto)
	{
		TryAutoSummon(InOwnerCharacter);
	}

	// 자동 스킬 사용
	if (CombatState == EAutoCombatState::Auto)
	{
		if (TryExcuteSkill(InOwnerCharacter))
		{
			if (OwnerController)
			{
				OwnerController->StopMovement();
			}
			return;
		}
	}

	// 기본공격 및 이동
	if (bInBasicAttackRange)
	{
		if (OwnerController)
		{
			OwnerController->StopMovement();
		}

		if (CurrentAttackCooldown <= 0.0f)
		{
			AttemptAction(InDeltaTime);
		}
	}
	else
	{
		if (CombatState == EAutoCombatState::Auto)
		{
			MoveToTarget(InDeltaTime);
		}
	}
}

void ULRCombatComponent::FindBestTarget()
{
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter) return;

	FGameplayTag EnemyRootTag = GetEnemyRootTag();
	if (!EnemyRootTag.IsValid()) return;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter);

	float CurrentCheckRadius = (CombatState == EAutoCombatState::Manual) ? AttackRange : SearchRadius;

	bool bOverlapFound = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), OwnerCharacter->GetActorLocation(), CurrentCheckRadius,
		ObjectTypes, ALRCharacter::StaticClass(), ActorsToIgnore, OutActors
	);

	AActor* ClosestEnemy = nullptr;
	float MinDistSq = FLT_MAX;

	for (AActor* Candidate : OutActors)
	{
		ALRCharacter* TargetChar = Cast<ALRCharacter>(Candidate);

		if (!TargetChar) continue;
		if (!TargetChar->GetUnitTag().MatchesTag(EnemyRootTag)) continue;
		if (IsTargetDead(TargetChar)) continue;

		float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), Candidate->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestEnemy = Candidate;
		}
	}

	if (ClosestEnemy)
	{
		CurrentTarget = ClosestEnemy;
		return;
	}

	if (CachedEnemyBase && IsValid(CachedEnemyBase))
	{
		CurrentTarget = CachedEnemyBase;
	}
	else
	{
		CurrentTarget = nullptr;
	}
}

void ULRCombatComponent::AttemptAction(float InDeltaTime)
{
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter) return;

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OwnerCharacter);
	if (!ASCInterface) return;

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("공격 실패: ASC가 NULL. Owner: %s"), *OwnerCharacter->GetName());
		return;
	}

	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = CurrentTarget;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter, LRTags::Ability_Combat_BasicShoot, EventData);

	LR_INFO(TEXT("공격 성공 / 타겟 : %s"), *CurrentTarget->GetName());
	CurrentAttackCooldown = 1.0f;
}

void ULRCombatComponent::MoveToTarget(float InDeltaTime)
{
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter || !CurrentTarget) return;

	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FVector MyLoc = OwnerCharacter->GetActorLocation();

	TargetLoc.Z = MyLoc.Z;

	FVector Direction = (TargetLoc - MyLoc).GetSafeNormal();
	OwnerCharacter->AddMovementInput(Direction, 1.0f);
}


// ============================================================================
// 헬퍼 함수
// ============================================================================

void ULRCombatComponent::UpdateTargetIndicator(ALRCharacter* InOwnerCharacter)
{
	UDecalComponent* TargetIndicator = InOwnerCharacter->FindComponentByClass<UDecalComponent>();
	if (!TargetIndicator) return;

	if (!CurrentTarget)
	{
		TargetIndicator->SetVisibility(false);
		return;
	}

	bool bInRange = IsTargetInRange();
	bool bShowIndicator = false;

	if (CombatState == EAutoCombatState::Auto)
	{
		bShowIndicator = true;
	}
	else
	{
		bShowIndicator = bInRange;
	}

	TargetIndicator->SetVisibility(bShowIndicator);

	if (bShowIndicator)
	{
		FVector TargetLoc = CurrentTarget->GetActorLocation();
		TargetLoc.Z -= 90.0f;
		TargetIndicator->SetWorldLocation(TargetLoc);
	}
}

void ULRCombatComponent::CheckAndClearDeadTarget()
{
	if (!CurrentTarget) return;

	bool bShouldDrop = false;

	if (!IsValid(CurrentTarget))
	{
		bShouldDrop = true;
	}
	else if (CurrentTarget->GetActorLocation().Z < -2000.0f)
	{
		bShouldDrop = true;
	}
	else if (IsTargetDead(CurrentTarget))
	{
		bShouldDrop = true;
	}

	if (bShouldDrop)
	{
		CurrentTarget = nullptr;
		ALRCharacter* OwnerCharacter = GetOwnerCharacter();
		if (OwnerCharacter && OwnerCharacter->GetController())
		{
			OwnerCharacter->GetController()->StopMovement();
		}
	}
}

bool ULRCombatComponent::IsTargetDead(AActor* InTargetActor) const
{
	if (!InTargetActor) return true;

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(InTargetActor);
	if (!ASCInterface) return false;

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return false;

	bool bFound = false;
	float Health = TargetASC->GetGameplayAttributeValue(ULRAttributeSet::GetHealthAttribute(), bFound);

	if (bFound && Health <= 0.0f)
	{
		return true;
	}

	return false;
}

ALRCharacter* ULRCombatComponent::GetOwnerCharacter() const
{
	return Cast<ALRCharacter>(GetOwner());
}

bool ULRCombatComponent::IsTargetInRange() const
{
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter || !CurrentTarget) return false;

	FVector MyLoc = OwnerCharacter->GetActorLocation();
	FVector TargetLoc = CurrentTarget->GetActorLocation();

	// 타겟의 콜리전을 가져와서 내 위치와 가장 가까운 '표면 좌표'를 구함!
	if (UPrimitiveComponent* TargetCollision = Cast<UPrimitiveComponent>(CurrentTarget->GetRootComponent()))
	{
		TargetCollision->GetClosestPointOnCollision(MyLoc, TargetLoc);
	}

	// 중심점이 아닌, 타겟의 '표면 좌표'와의 거리를 계산
	float DistSq = FVector::DistSquared(MyLoc, TargetLoc);
	float AttackRangeSq = AttackRange * AttackRange;

	return DistSq <= AttackRangeSq;

}

FGameplayTag ULRCombatComponent::GetEnemyRootTag() const
{
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (!OwnerCharacter) return FGameplayTag();

	FGameplayTag MyTag = OwnerCharacter->GetUnitTag();
	if (MyTag.MatchesTag(LRTags::Team_Player))      return LRTags::Team_Enemy;
	if (MyTag.MatchesTag(LRTags::Team_Enemy))       return LRTags::Team_Player;

	return FGameplayTag();
}

void ULRCombatComponent::ClearTarget()
{
	CurrentTarget = nullptr;
	ALRCharacter* OwnerCharacter = GetOwnerCharacter();
	if (OwnerCharacter && OwnerCharacter->GetController())
	{
		OwnerCharacter->GetController()->StopMovement();
	}
}

bool ULRCombatComponent::TryExcuteSkill(ALRCharacter* InOwnerCharacter)
{
	if (AutoSkillDelay > 0.0f)
	{
		return false;
	}

	if (!CurrentTarget) return false;

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(InOwnerCharacter);
	if (!ASCInterface) return false;
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

	ALRPlayerState* PS = InOwnerCharacter->GetPlayerState<ALRPlayerState>();
	if (!PS) return false;

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys) return false;

	// PlayerState에서 장착된 스킬 ID 목록을 받아옴
	TArray<FName> EquippedSkillIDs = PS->GetEquippedAutoSkillIDs();

	if (EquippedSkillIDs.Num() == 0)
	{
		//LR_WARN(TEXT("[TryExecuteSkill] 장착된 스킬 ID가 없음 PlayerState 확인 요망"));
		return false;
	}

	for (const FName& SkillID : EquippedSkillIDs)
	{
		const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);
		FGameplayTag SkillTag = SkillData.SkillTag;

		const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillData.SkillEffectID);
		float RealAttackRange = EffectData.Range;
		//LR_INFO(TEXT("[TryExecuteSkill] 검사 중인 스킬 ID: %s | 태그: %s | 사거리: %.1f"),
		//	*SkillID.ToString(), *SkillTag.ToString(), RealAttackRange);

		float DistSq = FVector::DistSquared(InOwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
		float RangeSq = RealAttackRange * RealAttackRange;

		if (DistSq <= RangeSq)
		{

			FGameplayEventData EventData;
			EventData.Instigator = InOwnerCharacter;
			EventData.Target = CurrentTarget;

			int32 TriggeredCount = ASC->HandleGameplayEvent(SkillTag, &EventData);

			if (TriggeredCount > 0)
			{
				//LR_INFO(TEXT("오토 스킬 발동 성공 태그: %s | 사거리: %.1f"), *SkillTag.ToString(), RealAttackRange);
				AutoSkillDelay = 2.0f;
				return true;
			}
			else
			{
				//LR_WARN(TEXT("[TryExecuteSkill] 발동 실패 : %s"), *SkillTag.ToString());
			}
		}
	}

	return false;
}

bool ULRCombatComponent::TryAutoSummon(ALRCharacter* InOwnerCharacter)
{
	ULRSummonComponent* SummonComp = InOwnerCharacter->FindComponentByClass<ULRSummonComponent>();
	if (!SummonComp) return false;

	TArray<FName> CurrentDeck = SummonComp->GetSummonDeck();
	if (CurrentDeck.Num() == 0) return false;

	for (int32 i = 0; i < CurrentDeck.Num(); ++i)
	{
		FName UnitID;
		const FCharacterStaticData* CharData = nullptr;

		if (SummonComp->IsValidSummonRequest(i, UnitID, CharData))
		{
			SummonComp->TrySummonUnit(i);

			//AutoSkillDelay = 1.0f;

			LR_INFO(TEXT("[Auto] %d번 슬롯 자동 소환 완료: %s"), i, *UnitID.ToString());
			return true;
		}
	}


	return false;
}
