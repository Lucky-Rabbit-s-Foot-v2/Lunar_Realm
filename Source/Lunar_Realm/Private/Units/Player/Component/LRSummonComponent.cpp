// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRSummonComponent.h"
#include "Units/Player/LRPlayerState.h"     
#include "Units/Member/LRMemberCharacter.h"
#include "Units/Member/LRMemberAIController.h"
#include "Structures/Core/LRPlayerCore.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ULRSummonComponent::ULRSummonComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULRSummonComponent::BeginPlay()
{
	Super::BeginPlay();
	FindPlayerCore();


}

void ULRSummonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULRSummonComponent::LoadDeckData(const TArray<FName>& InUnitIDs)
{
	SummonDeck = InUnitIDs;
	LR_INFO(TEXT("덱 로드완료, 총 유닛 수: %d"), SummonDeck.Num());
}

// ============================================================================
// 소환 메인 로직
// ============================================================================

void ULRSummonComponent::TrySummonUnit(int32 InSlotIndex)
{
	if (!TargetCore)
	{
		FindPlayerCore();
	}

	// 유효성 검사
	if (!IsSlotValid(InSlotIndex))
	{
		return;
	}

	FName UnitID = SummonDeck[InSlotIndex];

	// 데이터 조회
	const FCharacterStaticData* CharData = GetCharacterData(UnitID);
	if (!CharData)
	{
		return;
	}
	// 쿨타임 확인
	if (IsOnCooldown(UnitID, CharData->SummonCooldown))
	{
		return;
	}

	// 비용 확인
	if (!CanAffordSummon(CharData->SummonCost))
	{
		LR_WARN(TEXT("에테르 부족"));
		return;
	}

	// 실행
	DeductSummonCost(CharData->SummonCost);
	UpdateLastSummonTime(UnitID);
	ProcessSummon(*CharData);

	if (OnUnitSummoned.IsBound())
	{
		OnUnitSummoned.Broadcast(InSlotIndex, CharData->SummonCooldown);
	}

}


// ============================================================================
// 내부 로직
// ============================================================================

void ULRSummonComponent::ProcessSummon(const FCharacterStaticData& InCharData)
{
	UWorld* World = GetWorld();
	if (!World) return;

	UPoolingSubsystem* PoolSys = World->GetSubsystem<UPoolingSubsystem>();
	if (!PoolSys) return;

	FTransform SpawnTransform = CalculateSpawnTransform();
	ALRMemberCharacter* NewUnit = PoolSys->Spawn<ALRMemberCharacter>(BaseMemberClass, SpawnTransform);

	if (NewUnit)
	{
		if (NewUnit->GetController() == nullptr)
		{
			NewUnit->SpawnDefaultController();
		}

		if (ALRMemberAIController* AICon = Cast<ALRMemberAIController>(NewUnit->GetController()))
		{
			AICon->RestartAI();
		}
		NewUnit->InitCharacterData(InCharData.DataID);

		LR_INFO(TEXT("유닛 소환 성공: %s (Cost: %.0f)"), *InCharData.DataID.ToString(), InCharData.SummonCost);

	}
	else
	{
		LR_ERROR(TEXT("소환 실패: 풀링 시스템에서 유닛을 반환하지 못함."));
	}

}

// ============================================================================
// 핼퍼 함수
// ============================================================================

void ULRSummonComponent::FindPlayerCore()
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALRPlayerCore::StaticClass());
	if (FoundActor)
	{
		TargetCore = Cast<ALRPlayerCore>(FoundActor);
		LR_INFO(TEXT("플레이어 코어 연결 성공: %s"), *TargetCore->GetName());
	}
	else
	{
		LR_WARN(TEXT("현재 맵에서 'ALRPlayerCore'를 찾지 못했습니다. 소환 시 다시 시도합니다."));
	}
}

const FCharacterStaticData* ULRSummonComponent::GetCharacterData(FName InUnitID) const
{
	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI) return nullptr;

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return nullptr;

	return &DataSys->GetCharacterStaticData(InUnitID);
}

ALRPlayerState* ULRSummonComponent::GetPlayerState() const
{
	ALRCharacter* OwnerChar = Cast<ALRCharacter>(GetOwner());
	if (!OwnerChar) return nullptr;
	return OwnerChar->GetPlayerState<ALRPlayerState>();
}

ULRPlayerAttributeSet* ULRSummonComponent::GetAttributeSet() const
{
	ALRPlayerState* PS = GetPlayerState();
	return PS ? Cast<ULRPlayerAttributeSet>(PS->GetAttributeSet()) : nullptr;
}

bool ULRSummonComponent::IsSlotValid(int32 InSlotIndex) const
{
	if (!TargetCore)
	{
		LR_ERROR(TEXT("Core 미연결 상태에서 소환 시도"));
		return false;
	}
	if (!BaseMemberClass)
	{
		LR_ERROR(TEXT("BaseMemberClass 비어있음"));
		return false;
	}
	if (!SummonDeck.IsValidIndex(InSlotIndex))
	{
		LR_WARN(TEXT("유효하지 않은 슬롯 인덱스: %d"), InSlotIndex);
		return false;
	}
	return true;
}

bool ULRSummonComponent::IsOnCooldown(FName InUnitID, float InCoolDownTime) const
{
	if (LastSummonTimeMap.Contains(InUnitID))
	{
		double LastTime = LastSummonTimeMap[InUnitID];
		double CurrentTime = GetWorld()->GetTimeSeconds();
		double Elapsed = CurrentTime - LastTime;

		if (Elapsed < InCoolDownTime)
		{
			LR_WARN(TEXT("[%s] 쿨타임 중 (남은 시간: %.1f초)"), *InUnitID.ToString(), (InCoolDownTime - Elapsed));
			return true;
		}
	}
	return false;
}

bool ULRSummonComponent::CanAffordSummon(float InCost) const
{
	ULRPlayerAttributeSet* AS = GetAttributeSet();
	if (!AS) return false;

	float CurrentAether = AS->GetAether();
	return CurrentAether >= InCost;
}

void ULRSummonComponent::DeductSummonCost(float InCost)
{
	ALRPlayerState* PS = GetPlayerState();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (ASC)
	{
		ASC->ApplyModToAttribute(ULRPlayerAttributeSet::GetAetherAttribute(), EGameplayModOp::Additive, -InCost);
	}
}

void ULRSummonComponent::UpdateLastSummonTime(FName InUnitID)
{
	LastSummonTimeMap.Add(InUnitID, GetWorld()->GetTimeSeconds());
}

FTransform ULRSummonComponent::CalculateSpawnTransform() const
{
	if (!TargetCore) return FTransform::Identity;

	FVector SpawnLocation = TargetCore->GetRandomSpawnLocation();
	SpawnLocation.Z = TargetCore->GetActorLocation().Z;

	FRotator SpawnRotation = FRotator(0.0f, 90.0f, 0.0f);

	return FTransform(SpawnRotation, SpawnLocation);
}

float ULRSummonComponent::GetRemainingCooldown(FName InUnitID) const
{
	if (LastSummonTimeMap.Contains(InUnitID))
	{
		double LastTime = LastSummonTimeMap[InUnitID];
		double CurrentTime = GetWorld()->GetTimeSeconds();
		double Elapsed = CurrentTime - LastTime;

		return 0.0f;
	}
	return 0.0f;
}



