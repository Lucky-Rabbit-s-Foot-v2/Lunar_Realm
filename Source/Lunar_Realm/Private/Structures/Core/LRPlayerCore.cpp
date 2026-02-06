// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LRPlayerCore.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

ALRPlayerCore::ALRPlayerCore()
{
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(RootComponent);
	SpawnArea->SetBoxExtent(FVector(100.0f, 300.0f, 100.0f));
	SpawnArea->SetRelativeLocation(FVector(300.0f, 0.0f, 0.0f));

	SpawnArea->SetCollisionProfileName(TEXT("NoCollision"));
	SpawnArea->SetHiddenInGame(false);
}

FVector ALRPlayerCore::GetRandomSpawnLocation() const
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector RandomPointInLocal = UKismetMathLibrary::RandomPointInBoundingBox(FVector::ZeroVector, BoxExtent);

	return SpawnArea->GetComponentTransform().TransformPosition(RandomPointInLocal);
}

