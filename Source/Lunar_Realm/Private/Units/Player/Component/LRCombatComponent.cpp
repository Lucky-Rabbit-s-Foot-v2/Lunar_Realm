// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRCombatComponent.h"
#include "Units/LRCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameplayTagsManager.h"
#include "Data/LRDataStructs.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRCombatComponent::ULRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SearchRadius = 1000.0f;
	AttackRange = 150.0f;
}


void ULRCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void ULRCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	if (!CurrentTarget || !CurrentTarget->IsValidLowLevel())
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
}

void ULRCombatComponent::SetAutoMode(bool bEnableAuto)
{
	CombatState = bEnableAuto ? EAutoCombatState::Auto : EAutoCombatState::Manual;

	UE_LOG(LogTemp, Log, TEXT("전투모드 변경: %s"), bEnableAuto ? TEXT("AUTO") : TEXT("MANUAL"));

	// [추가] 수동으로 전환될 때, 혹시 남아있을지 모르는 이동/정지 명령 초기화
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

	// 주변 스캔
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 캐릭터만 검색

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerChar); // 나는 제외

	bool bFound = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		OwnerChar->GetActorLocation(),
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

	// 가장 가까운 Enemy 보유자 찾기
	AActor* ClosestEnemy = nullptr;
	float MinDistSq = FLT_MAX;

	for (AActor* Candidate : OutActors)
	{
		ALRCharacter* TargetChar = Cast<ALRCharacter>(Candidate);
		if (!TargetChar) continue;


		if (TargetChar->GetUnitTag().MatchesTag(EnemyRootTag))
		{
			// 거리 비교 (가장 가까운 놈 찾기)
			float DistSq = FVector::DistSquared(OwnerChar->GetActorLocation(), Candidate->GetActorLocation());
			if (DistSq < MinDistSq)
			{
				MinDistSq = DistSq;
				ClosestEnemy = Candidate;
			}
		}
	}

	// 최종 타겟 설정
	CurrentTarget = ClosestEnemy;

	if (CurrentTarget)
	{
		UE_LOG(LogTemp, Log, TEXT("타겟 발견! 추적 시작: %s"), *CurrentTarget->GetName());
	}


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
	FVector TargetLoc = CurrentTarget->GetActorLocation();
	FVector MyLoc = OwnerChar->GetActorLocation();

	// 이동 입력 넣기
	TargetLoc.Z = MyLoc.Z;

	FVector Direction = (TargetLoc - MyLoc).GetSafeNormal();

	// [디버그] 방향이 제대로 나오는지 로그 확인 (이동 안되면 주석 풀고 확인)
	UE_LOG(LogTemp, Log, TEXT("Move To: %s, Dir: %s"), *CurrentTarget->GetName(), *Direction.ToString());

	// 2. 이동 입력 넣기 (NavMesh 경고랑 상관없이, 이건 무조건 밀어줌)
	OwnerChar->AddMovementInput(Direction, 1.0f);
}

