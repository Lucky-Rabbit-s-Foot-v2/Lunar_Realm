// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LRCore.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "System/LoggingSystem.h"

// Sets default values
ALRCore::ALRCore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollision"));
	SetRootComponent(HitCollision);

	HitCollision->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	HitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCollision->SetGenerateOverlapEvents(true);

	HitCollision->SetCollisionObjectType(ECC_WorldStatic);

	HitCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(HitCollision);

	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// TEMP : 피격 or 접근 감지 훅
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &ALRCore::OnHitCollisionBeginOverlap);
}

void ALRCore::BeginPlay()
{
}

void ALRCore::OnHitCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		LR_DEBUG(TEXT("Core Overlapped By: %s"), *OtherActor->GetName());
		return;
	}
}

