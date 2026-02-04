// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRSummonComponent.h"
#include "Units/Member/LRMemberCharacter.h"
#include "Structures/Core/LRPlayerCore.h"
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

	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALRPlayerCore::StaticClass());
	if (FoundActor)
	{
		TargetCore = Cast<ALRPlayerCore>(FoundActor);
		UE_LOG(LogTemp, Log, TEXT("플레이어 코어 찾음: %s"), *TargetCore->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("플레이어코어가 없어 소환할 수 없음"));
	}
}

void ULRSummonComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULRSummonComponent::LoadDeckData(const TArray<int32>& UnitIDs)
{
	SummonDeck.Empty();
	for (int32 ID : UnitIDs)
	{
		UE_LOG(LogTemp, Log, TEXT("덱 로드중 ID: %d"), ID);

		//[TODO] DataManager를 써서 ID
		// 예: TSubclassOf<ALRSubCharacter> UnitClass = DataManager->GetUnitClass(ID);
		// if (UnitClass) SummonDeck.Add(UnitClass);

		UE_LOG(LogTemp, Warning, TEXT("덱 로드완료, 총 유닛 수: %d"), UnitIDs.Num());
	}
}

void ULRSummonComponent::TrySummonUnit(int32 SlotIndex)
{
	if (SummonDeck.IsValidIndex(SlotIndex) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("슬롯 %d가 비어있음"), SlotIndex);
		return;
	}

	TSubclassOf<ALRMemberCharacter> UnitClass = SummonDeck[SlotIndex];
	if (!UnitClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("슬롯 %d에 유닛 클래스가 없음"), SlotIndex);
		return;
	}
	if (TargetCore == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Summon Failed: No TargetCore Linked! (Did BeginPlay find the Core?)"));
		return;
	}

	FVector SpawnLocation = TargetCore->GetRandomSpawnLocation();
	SpawnLocation.Z = TargetCore->GetActorLocation().Z;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if(UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		World->SpawnActor<ALRMemberCharacter>(UnitClass, SpawnLocation, SpawnRotation, SpawnParams);

		UE_LOG(LogTemp, Log, TEXT("Summon Success! Slot: %d"), SlotIndex);
	}
}


