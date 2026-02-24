// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
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
		UE_LOG(LogTemp, Log, TEXT("[Combat] 적 기지 캐싱 완료: %s"), *CachedEnemyBase->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] 맵에 'Enemy.Structure.Core' 태그를 가진 액터가 없음."));
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

void ULRCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentAttackCooldown > 0.0f)
	{
		CurrentAttackCooldown -= DeltaTime;
	}

	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}


	UpdateTargetIndicator(OwnerCharacter);

	if (CurrentTarget)
	{
		if (CurrentTarget->GetActorLocation().Z < -2000.0f)
		{
			return;
		}
		ProcessCombatLogic(OwnerCharacter, DeltaTime);
	}
}


void ULRCombatComponent::UpdateTargetIndicator(ALRCharacter* OwnerCharacter)
{
	UDecalComponent* TargetIndicator = OwnerCharacter->FindComponentByClass<UDecalComponent>();
	if (!TargetIndicator) return; 


	if (!CurrentTarget)
	{
		TargetIndicator->SetVisibility(false);
		return;
	}


	float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	float AttackRangeSq = AttackRange * AttackRange;
	bool bInRange = (DistSq <= AttackRangeSq);


	bool bShowIndicator = false;

	if (CombatState == EAutoCombatState::Auto) // 자동
	{
		bShowIndicator = true; 
	}
	else // 수동
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

void ULRCombatComponent::ProcessCombatLogic(ALRCharacter* OwnerCharacter, float DeltaTime)
{
	float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	float AttackRangeSq = AttackRange * AttackRange;
	bool bInRange = (DistSq <= AttackRangeSq);

	AController* OwnerController = OwnerCharacter->GetController();

	if (bInRange) 
	{
		if (OwnerController)
		{
			OwnerController->StopMovement();
		}


		if (CurrentAttackCooldown <= 0.0f)
		{
			AttemptAction(DeltaTime);
		}
	}
	else 
	{
		if (CombatState == EAutoCombatState::Auto)
		{
			MoveToTarget(DeltaTime);
		}
	}
}


void ULRCombatComponent::SetAutoMode(bool bEnableAuto)
{
	CombatState = bEnableAuto ? EAutoCombatState::Auto : EAutoCombatState::Manual;
	UE_LOG(LogTemp, Log, TEXT("전투모드 변경: %s"), bEnableAuto ? TEXT("AUTO") : TEXT("MANUAL"));

	if (!bEnableAuto)
	{
		ALRCharacter* OwnerCharactor = Cast<ALRCharacter>(GetOwner());
		if (OwnerCharactor && OwnerCharactor->GetController())
		{
			OwnerCharactor->GetController()->StopMovement();
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

	UE_LOG(LogTemp, Log, TEXT("무기설정 ID: %s, Range: %.1f"), *InWeaponID.ToString(), AttackRange);
}


void ULRCombatComponent::OnCombatLogicTimer()
{
	if (CurrentTarget)
	{
		if (!IsValid(CurrentTarget) || IsTargetDead(CurrentTarget))
		{
			CurrentTarget = nullptr;
		}
	}

	CheckAndClearDeadTarget();

	bool bIsManualMode = (CombatState == EAutoCombatState::Manual);
	bool bIsTargetingCore = (CurrentTarget == CachedEnemyBase);

	if (!CurrentTarget || bIsManualMode || bIsTargetingCore)
	{
		FindBestTarget();
	}
}


bool ULRCombatComponent::IsTargetDead(AActor* TargetActor) const
{
	if (!TargetActor) return true;

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	if (!ASCInterface) return false; 

	UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
	if (!TargetASC) return false;

	bool bFound = false;
	// TODO_BJM : 어트리뷰트셋 베이스 만들면 연결 변경해야함
	float Health = TargetASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetHealthAttribute(), bFound);


	if (bFound && Health <= 0.0f)
	{
		return true;
	}

	return false;
}

void ULRCombatComponent::FindBestTarget()
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetOwner());
	if (!OwnerChar) return;


	FGameplayTag MyTag = OwnerChar->GetUnitTag();
	FGameplayTag EnemyRootTag;

	if (MyTag.MatchesTag(LRTags::Team_Player))      EnemyRootTag = LRTags::Team_Enemy;
	else if (MyTag.MatchesTag(LRTags::Team_Enemy)) EnemyRootTag = LRTags::Team_Player;
	else return; 

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerChar);

	bool bOverlapFound = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(), OwnerChar->GetActorLocation(), SearchRadius,
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

		float DistSq = FVector::DistSquared(OwnerChar->GetActorLocation(), Candidate->GetActorLocation());
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


void ULRCombatComponent::AttemptAction(float DeltaTime) 
{
	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OwnerCharacter);
	if (!ASCInterface) return; 

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{

		UE_LOG(LogTemp, Error, TEXT("[Combat] 공격 실패: ASC가 NULL입니다. Owner: %s"), *OwnerCharacter->GetName());
		return;
	}

	//260219 KHS. Instigator / Target담아서 능력 발동하도록 수정
	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = CurrentTarget;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwnerCharacter, LRTags::Ability_Combat_BasicShoot, EventData);
	
	UE_LOG(LogTemp, Log, TEXT("공격 성공 / 타겟 : %s"), *CurrentTarget->GetName());
	CurrentAttackCooldown = 1.0f;
}

void ULRCombatComponent::MoveToTarget(float DeltaTime)
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetOwner());

	if (!OwnerChar || !CurrentTarget) return;

	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FVector MyLoc = OwnerChar->GetActorLocation();

	TargetLoc.Z = MyLoc.Z;

	FVector Direction = (TargetLoc - MyLoc).GetSafeNormal();
	OwnerChar->AddMovementInput(Direction, 1.0f);
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
		ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
		if (OwnerCharacter && OwnerCharacter->GetController())
		{
			OwnerCharacter->GetController()->StopMovement();
		}
	}
}