// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRSummonComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/Member/LRMemberCharacter.h"
#include "Structures/Core/LRPlayerCore.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"

ULRSummonComponent::ULRSummonComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULRSummonComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALRPlayerCore::StaticClass());
	if (FoundActor)
	{
		TargetCore = Cast<ALRPlayerCore>(FoundActor);
		LR_INFO(TEXT("플레이어 코어 연결 성공: %s"), *TargetCore->GetName());
	}
	else
	{
		LR_ERROR(TEXT("맵에 플레이어 코어(ALRPlayerCore)가 없어 소환할 수 없습니다!"));
	}

	// 테스트용
	SummonDeck.Empty();
	SummonDeck.Add(FName(TEXT("1"))); // 0번 슬롯: 데이지
	SummonDeck.Add(FName(TEXT("2"))); // 1번 슬롯: 링크
	SummonDeck.Add(FName(TEXT("3"))); // 2번 슬롯: 간달프

	LR_INFO(TEXT("C++에서 덱 강제 장전 완료! 총 유닛 수: %d"), SummonDeck.Num());

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

void ULRSummonComponent::TrySummonUnit(int32 InSlotIndex)
{
	if (!IsSummonValid(InSlotIndex))
	{
		return;
	}

	FName TargetUnitID = SummonDeck[InSlotIndex];

	ProcessSummon(TargetUnitID);
}

bool ULRSummonComponent::IsSummonValid(int32 InSlotIndex) const
{
	if (!TargetCore)
	{
		LR_ERROR(TEXT("소환 실패 : TargetCore가 연결되지 않음"));
		return false;
	}
	if (!SummonDeck.IsValidIndex(InSlotIndex))
	{
		LR_WARN(TEXT("소환 실패: 슬롯 %d가 유효하지 않습니다."), InSlotIndex);
		return false;
	}

	// BaseMemberClass가 블루프린트에서 설정되어 있는지 방어 코드
	if (!BaseMemberClass)
	{
		LR_ERROR(TEXT("소환 실패: 컴포넌트의 BaseMemberClass가 비어있습니다! 블루프린트를 확인하세요."));
		return false;
	}
	return true;
}

void ULRSummonComponent::ProcessSummon(FName InTargetUnitID)
{
	UWorld* World = GetWorld();
	if (!World) return;

	UPoolingSubsystem* PoolSys = World->GetSubsystem<UPoolingSubsystem>();
	if (!PoolSys) return;

	FVector SpawnLocation = TargetCore->GetRandomSpawnLocation();
	SpawnLocation.Z = TargetCore->GetActorLocation().Z;
	FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

	ALRMemberCharacter* NewUnit = PoolSys->Spawn<ALRMemberCharacter>(BaseMemberClass, SpawnTransform);

	if (NewUnit)
	{
		NewUnit->InitCharacterData(InTargetUnitID);

		LR_INFO(TEXT("유닛 소환 성공 ID: %s"), *InTargetUnitID.ToString());
	}
	else
	{
		LR_ERROR(TEXT("소환 실패: 풀링 시스템에서 유닛을 반환하지 못했습니다."));
	}



}


