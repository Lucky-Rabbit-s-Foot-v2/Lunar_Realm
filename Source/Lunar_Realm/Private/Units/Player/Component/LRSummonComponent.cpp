// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/Component/LRSummonComponent.h"
#include "GameFramework/Character.h"

ULRSummonComponent::ULRSummonComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void ULRSummonComponent::BeginPlay()
{
	Super::BeginPlay();

	
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
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("소유자가 캐릭터가 아님"));
		return;
	}
	FVector SpawnLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * SpawnDistance;
	FRotator SpawnRotation = OwnerCharacter->GetActorRotation();

	if(UWorld* World = GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		World->SpawnActor<ALRMemberCharacter>(UnitClass, SpawnLocation, SpawnRotation, SpawnParams);

		UE_LOG(LogTemp, Log, TEXT("Summon Success! Slot: %d"), SlotIndex);
	}
}


