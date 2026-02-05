// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "GameplayTagsManager.h"

ULRCombatComponent::ULRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void ULRCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void ULRCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter || !CurrentTarget->IsValidLowLevel())
	{
		FindBestTarget();
	}

	if (CurrentTarget)
	{
		float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
		float AttackRangeSq = AttackRange * AttackRange;

		if (DistSq <= AttackRangeSq)
		{
			TryAction(DeltaTime);
			
			if (CombatState == EAutoCombatState::Auto)
			{
				OwnerCharacter->GetController()->StopMovement();
			}
			else
			{
				if (CombatState == EAutoCombatState::Auto)
				{
					MoveToTarget(DeltaTime);
				}
			}
		}
	}
}

void ULRCombatComponent::SetAutoMode(bool bEnableAuto)
{
	CombatState = bEnableAuto ? EAutoCombatState::Auto : EAutoCombatState::Manual;

	UE_LOG(LogTemp, Log, TEXT("전투모드 변경: %s"), bEnableAuto ? TEXT("AUTO") : TEXT("MANUAL"));
}

void ULRCombatComponent::UpdateWeaponInfo(int32 InWeaponID)
{
	// 장비 ID 파싱 로직
	// ID Format: T CC III SS (예: 2 01 001 02)
	int32 Category = (InWeaponID / 100000) % 100;
	if (Category == 1)
	{
		AttackRange = 150.0f; 
	}
	else if (Category == 2)
	{
		AttackRange = 800.0f; 
	}
	else
	{
		AttackRange = 100.0f;
	}

	UE_LOG(LogTemp, Log, TEXT("무기설정 ID: %d, Category: %02d, Range: %.1f"), InWeaponID, Category, AttackRange);
}

void ULRCombatComponent::FindBestTarget()
{
	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	FGameplayTag MyTag = OwnerCharacter->GetUnitTag();
	FGameplayTag EnemyRootTag;

	if (MyTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player"))))
	{
		EnemyRootTag = FGameplayTag::RequestGameplayTag(FName("Enemy"));
	}
	else if (MyTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Enemy"))))
	{
		EnemyRootTag = FGameplayTag::RequestGameplayTag(FName("Player"));
	}
	else
	{
		return;
	}

	// 주변 스캔
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 폰만

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerCharacter);

	bool bFound = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		OwnerCharacter->GetActorLocation(),
		SearchRadius,
		ObjectTypes,
		ALRCharacter::StaticClass(),
		ActorsToIgnore,
		OutActors
	);

	if (!bFound)
	{
		CurrentTarget = nullptr;
		return;
	}

	AActor* ClosestEnemy = nullptr;
	float MinDistSq = FLT_MAX;

	for (AActor* Candidate : OutActors)
	{
		ALRCharacter* TargetCharacter = Cast<ALRCharacter>(Candidate);
		if (!TargetCharacter) continue;

		// 상대 태그가 EnemyRootTag를 포함하는가?
		if (TargetCharacter->GetUnitTag().MatchesTag(EnemyRootTag))
		{
			float DistSq = FVector::DistSquared(OwnerCharacter->GetActorLocation(), Candidate->GetActorLocation());
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				ClosestEnemy = Candidate;
			}
		}
	}

	CurrentTarget = ClosestEnemy;

}

void ULRCombatComponent::TryAction(float DeltaTime)
{
	if (CurrentAttackCooldown > 0.0f)
	{
		CurrentAttackCooldown -= DeltaTime; // 쿨타임 감소
		return;
	}

	// 공격 실행 로그
	UE_LOG(LogTemp, Warning, TEXT("[Action] Attack! Target: %s"), *CurrentTarget->GetName());

	// TODO: 여기서 실제 공격 애니메이션 몽타주 재생 or 데미지 전달

	CurrentAttackCooldown = 1.0f; 
}

void ULRCombatComponent::MoveToTarget(float DeltaTime)
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetOwner());
	if (!OwnerChar || !CurrentTarget) return;

	// 타겟 방향 구하기
	FVector Direction = (CurrentTarget->GetActorLocation() - OwnerChar->GetActorLocation()).GetSafeNormal();

	// 이동 입력 넣기
	OwnerChar->AddMovementInput(Direction, 1.0f);
}

