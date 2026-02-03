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

	if (PooledActor)
	{
		ActiveActors.Add(PooledActor);
		if (PooledActor->Implements<ULRPoolableInterface>())
		{
			ILRPoolableInterface::Execute_OnPoolActivate(PooledActor);
		}
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
	for (int32 i = 0; i < Count; ++i)
	{
		AActor* NewActor = SpawnPooledActor(ClassToInit, FTransform::Identity);
		ReturnToPool(NewActor);
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
