// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameplayTagsManager.h"
#include "Data/LRDataStructs.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
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

	if (!CurrentTarget) return;

	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	float AttackRangeSq = AttackRange * AttackRange;

	if (DistSq <= AttackRangeSq)
	{
		TryAction(DeltaTime);

		if (CombatState == EAutoCombatState::Auto)
		{
			if (OwnerCharacter->GetController())
			{
				OwnerCharacter->GetController()->StopMovement();
			}
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

	// 수동으로 전환될 때, 혹시 남아있을지 모르는 이동/정지 명령 초기화
	if (!bEnableAuto)
	{
		ALRCharacter* OwnerCharactor = Cast<ALRCharacter>(GetOwner());
		if (OwnerCharactor && OwnerCharactor->GetController())
		{
			OwnerCharactor->GetController()->StopMovement(); // 일단 멈추고 유저 입력 대기
		}
	}
}

void ULRCombatComponent::UpdateWeaponInfo(int32 InWeaponID)
{
	FEntityIDInfo ParsingInfo(InWeaponID);

	ELRItemType ItemType = ParsingInfo.GetItemType();

	if (ItemType == ELRItemType::MELEE) // 근거리
	{
		AttackRange = 150.0f;
	}
	else if (ItemType == ELRItemType::RANGED) // 원거리
	{
		AttackRange = 800.0f;
	}
	else
	{
		AttackRange = 100.0f;
	}

	UE_LOG(LogTemp, Log, TEXT("무기설정 ID: %d, Type: %d, Range: %.1f"), InWeaponID, (int32)ItemType, AttackRange);
}

void ULRCombatComponent::OnCombatLogicTimer()
{
	if (CurrentTarget)
	{
		bool bShouldClearTarget = false;

		if (!IsValid(CurrentTarget))
		{
			bShouldClearTarget = true;
		}
		else
		{
			IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(CurrentTarget);
			if (ASCInterface)
			{
				UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
				if (TargetASC)
				{
					bool bFound = false;
					float Health = TargetASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetHealthAttribute(), bFound);
					if (bFound && Health <= 0.0f)
					{
						bShouldClearTarget = true;
					}
				}
			}
		}

		if (bShouldClearTarget)
		{
			CurrentTarget = nullptr;
		}
	}

	if (!CurrentTarget && CombatState == EAutoCombatState::Auto)
	{
		FindBestTarget();
	}
}

void ULRCombatComponent::FindBestTarget()
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetOwner());
	if (!OwnerChar) return;

	FGameplayTag MyTag = OwnerChar->GetUnitTag();
	FGameplayTag EnemyRootTag;

	if (MyTag.MatchesTag(LRTags::Team_Player))
	{
		EnemyRootTag = LRTags::Team_Enemy; 
	}
	else if (MyTag.MatchesTag(LRTags::Team_Enemy))
	{
		EnemyRootTag = LRTags::Team_Player;
	}
	else
	{
		return;
	}

// 1순위: 적 검색

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
		
		bool bIsDead = false;
		IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetChar);
		if (ASCInterface)
		{
			UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
			if (TargetASC)
			{
				bool bAttributeExists = false;
				float Health = TargetASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetHealthAttribute(), bAttributeExists);
				if (bAttributeExists && Health <= 0.0f)
				{
					bIsDead = true;
				}
			}
		}

		if (bIsDead) continue;

		if (TargetChar->GetUnitTag().MatchesTag(EnemyRootTag))
		{

			float DistSq = FVector::DistSquared(OwnerChar->GetActorLocation(), Candidate->GetActorLocation());
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				ClosestEnemy = Candidate;
			}
		}
	}

	if (ClosestEnemy)
	{
		CurrentTarget = ClosestEnemy;
		UE_LOG(LogTemp, Log, TEXT("[FindTarget] 적 발견: %s"), *CurrentTarget->GetName());
		return;
	}

	// 2순위: 적 기지 검색

	// TODO : Actor Tag로할지 ASC Tag로할지 협의 필요
	TArray<AActor*> AllBases;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy.Structure.Core"), AllBases);

	if (AllBases.Num() > 0)
	{
		CurrentTarget = AllBases[0];
		UE_LOG(LogTemp, Log, TEXT("[FindTarget] 기지 발견! 개수: %d, 타겟: %s"), AllBases.Num(), *CurrentTarget->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[FindTarget] 주변 적도 없고, 'Enemy.Structure.Core' 태그된 기지도 못 찾음! (개수: 0)"));
		CurrentTarget = nullptr;
	}

}

void ULRCombatComponent::TryAction(float DeltaTime)
{
	if (CurrentAttackCooldown > 0.0f)
	{
		CurrentAttackCooldown -= DeltaTime;
		return;
	}
	
	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter) return;
	
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OwnerCharacter);
	if (ASCInterface)
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
		
		if (ASC)
		{
			FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Combat.BasicShoot"));
			
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(AttackTag);
			
			if (ASC->TryActivateAbilitiesByTag(TagContainer))
			{
				UE_LOG(LogTemp, Warning, TEXT("[Action] Attack! Target: %s"), *CurrentTarget->GetName());
				CurrentAttackCooldown = 1.0f; 
			}
			else
			{
			}
		}
	}
	// TODO: 여기서 실제 공격 애니메이션 몽타주 재생 or 데미지 전달

}

void ULRCombatComponent::MoveToTarget(float DeltaTime)
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetOwner());
	if (!OwnerChar || !CurrentTarget) return;

	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FVector MyLoc = OwnerChar->GetActorLocation();

	TargetLoc.Z = MyLoc.Z;

	FVector Direction = (TargetLoc - MyLoc).GetSafeNormal();

	UE_LOG(LogTemp, Log, TEXT("Move To: %s, Dir: %s"), *CurrentTarget->GetName(), *Direction.ToString());

	OwnerChar->AddMovementInput(Direction, 1.0f);
}

