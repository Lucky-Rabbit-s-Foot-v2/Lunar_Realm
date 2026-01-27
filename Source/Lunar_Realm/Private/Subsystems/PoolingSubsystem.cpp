// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PoolingSubsystem.h"
#include "GameFramework/Actor.h"

AActor* UPoolingSubsystem::SpawnPooledActor(TSubclassOf<AActor> ClassToSpawn, const FTransform& SpawnTransform)
{
	if (!ClassToSpawn)
	{
		return nullptr;
	}

	AActor* PooledActor = nullptr;
	TArray<AActor*>* PoolStack = ActorPool.Find(ClassToSpawn);
	if (PoolStack && PoolStack->Num() > 0)
	{
		while (PoolStack->Num() > 0)
		{
			PooledActor = PoolStack->Pop();
			if (IsValid(PooledActor))
			{
				break;
			}
		}
	}

	if (!IsValid(PooledActor))
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		PooledActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTransform, SpawnParams);
	}
	else
	{
		// TODO: 할당된 액터 위치/회전 등 재설정 및 활성화 진행
	}

	if (PooledActor && PooledActor->Implements<ULRPoolableInterface>())
	{
		ILRPoolableInterface::Execute_OnPoolActivate(PooledActor);
	}

	return PooledActor;
}

void UPoolingSubsystem::ReturnToPool(AActor* ActorToReturn)
{
	if (!IsValid(ActorToReturn))
	{
		return;
	}

	// TODO: 비활성화 로직 진행

	if (ActorToReturn->Implements<ULRPoolableInterface>())
	{
		ILRPoolableInterface::Execute_OnPoolDeactivate(ActorToReturn);
	}

	UClass* ActorClass = ActorToReturn->GetClass();
	TArray<AActor*>& PoolStack = ActorPool.FindOrAdd(ActorClass);
	PoolStack.Add(ActorToReturn);
}

void UPoolingSubsystem::InitializePool(TSubclassOf<AActor> ClassToInit, int32 Count)
{
	for (int32 i = 0; i < Count; ++i)
	{
		AActor* NewActor = SpawnPooledActor(ClassToInit, FTransform::Identity);
		ReturnToPool(NewActor);
	}
}
