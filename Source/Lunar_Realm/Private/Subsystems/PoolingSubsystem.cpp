// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/PoolingSubsystem.h"
#include "GameFramework/Actor.h"

#include "System/LoggingSystem.h"

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
		// LR_INFO(TEXT("Spawn New Actor")); // Enemy로 인해 너무 많이 출력되서 임시로 주석처리 필요시 해제하고 사용 바람
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		PooledActor = GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTransform, SpawnParams);
	}

	if (PooledActor)
	{
		PooledActor->SetActorTransform(SpawnTransform);

		if (PooledActor->Implements<ULRPoolableInterface>())
		{
			ILRPoolableInterface::Execute_OnPoolActivate(PooledActor);
		}
		
		ActiveActors.Add(PooledActor);
	}

	return PooledActor;
}

void UPoolingSubsystem::ReturnToPool(AActor* ActorToReturn)
{
	if (!IsValid(ActorToReturn))
	{
		return;
	}

	ActiveActors.Remove(ActorToReturn);

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
	TArray<AActor*> CachedActors;

	for (int32 i = 0; i < Count; ++i)
	{
		AActor* NewActor = SpawnPooledActor(ClassToInit, FTransform::Identity);
		CachedActors.Add(NewActor);
	}

	for (AActor* CachedActor : CachedActors)
	{
		ReturnToPool(CachedActor);
	}
}

void UPoolingSubsystem::ReturnAllActiveActors()
{
	TArray<AActor*> ActorsToReturn = ActiveActors.Array();

	ActiveActors.Empty();

	for (AActor* Actor : ActorsToReturn)
	{
		if (IsValid(Actor))
		{
			if (Actor->Implements<ULRPoolableInterface>())
			{
				ILRPoolableInterface::Execute_OnPoolDeactivate(Actor);
			}
			UClass* ActorClass = Actor->GetClass();
			ActorPool.FindOrAdd(ActorClass).Push(Actor);
		}
	}
}

void UPoolingSubsystem::ClearAllPools()
{
	ReturnAllActiveActors();

	for (auto& PoolPair : ActorPool)
	{
		TArray<AActor*>& PoolStack = PoolPair.Value;
		for (AActor* PooledActor : PoolStack)
		{
			if (IsValid(PooledActor))
			{
				PooledActor->Destroy();
			}
		}
	}

	ActorPool.Empty();
	ActiveActors.Empty();
}
