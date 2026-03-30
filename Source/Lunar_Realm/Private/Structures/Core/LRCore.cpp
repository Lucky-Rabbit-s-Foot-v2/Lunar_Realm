// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LRCore.h"
#include "GAS/Attributes/LRCoreAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "System/LoggingSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
ALRCore::ALRCore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	//AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AttributeSet = CreateDefaultSubobject<ULRCoreAttributeSet>(TEXT("AttributeSet"));

	HitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HitCollision"));
	SetRootComponent(HitCollision);

	HitCollision->SetBoxExtent(FVector(2605.0f, 175.0f, 200.0f));
	HitCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCollision->SetGenerateOverlapEvents(true);

	HitCollision->SetCollisionObjectType(ECC_WorldDynamic);
	//HitCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	HitCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	HitCollision->SetCanEverAffectNavigation(false);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(HitCollision);

	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// TEMP : 피격 or 접근 감지 훅
	HitCollision->OnComponentBeginOverlap.AddDynamic(this, &ALRCore::OnHitCollisionBeginOverlap);

	DustNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DustNiagaraComp"));
	DustNiagaraComp->SetupAttachment(RootComponent);
	DustNiagaraComp->bAutoActivate = false;
	DustNiagaraComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	DustNiagaraComp2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DustNiagaraComp2"));
	DustNiagaraComp2->SetupAttachment(RootComponent);
	DustNiagaraComp2->bAutoActivate = false;
	DustNiagaraComp2->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));


}

void ALRCore::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	if (AbilitySystemComponent && AttributeSet)
	{
		AttributeSet->InitMaxHealth(MaxCoreHealth);
		AttributeSet->InitHealth(MaxCoreHealth);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			ULRAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRCore::OnHealthChanged);

		LR_INFO(TEXT("[%s] GAS 초기화 완료. 현재 체력: %.f"), *GetName(), AttributeSet->GetHealth());
	}
	//if (AbilitySystemComponent)
	//{
	//	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	//		AttributeSet->GetHealthAttribute()).AddUObject(this, &ALRCore::OnHealthChanged);

	//	LR_INFO(TEXT("[%s] GAS 초기화 완료. 현재 체력: %.f"), *GetName(), AttributeSet->GetHealth());
	//}
}

void ALRCore::OnHitCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	LR_DEBUG(TEXT("Core Overlapped By: %s"), *OtherActor->GetName());
}

void ALRCore::UpdateCollapseSequence()
{
	if (!VisualMesh) return;

	CollapseElapsedTime += 0.02f; 

	if (CollapseElapsedTime >= 5.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CollapseUpdateTimerHandle);
		VisualMesh->SetVisibility(false);
		return;
	}

	float ShakeIntensity = FMath::Lerp(5.0f, 1.0f, CollapseElapsedTime / 5.0f);
	FVector ShakeOffset(
		FMath::RandRange(-ShakeIntensity, ShakeIntensity),
		FMath::RandRange(-ShakeIntensity, ShakeIntensity),
		FMath::RandRange(-ShakeIntensity, ShakeIntensity)
	);

	float SinkAmount = FMath::Lerp(0.0f, -500.0f, CollapseElapsedTime / 5.0f);
	FVector SinkOffset(0.0f, 0.0f, SinkAmount);

	VisualMesh->SetRelativeLocation(InitialMeshLocation + SinkOffset + ShakeOffset);
}

void ALRCore::OnHealthChanged(const FOnAttributeChangeData& InData)
{
	float NewHealth = InData.NewValue;
	LR_INFO(TEXT("[%s] 체력 변경 : %.1f"), *GetName(), NewHealth);

	if (NewHealth <= 0.0f)
	{
		OnCoreDestroyed();
	}
}

void ALRCore::OnCoreDestroyed()
{
	if (bIsDestroyed) return;
	bIsDestroyed = true;

	LR_WARN(TEXT("[%s] 코어 파괴"), *GetName());

	if (HitCollision)
	{
		HitCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HitCollision->SetGenerateOverlapEvents(false);
	}

	if (VisualMesh)
	{

		InitialMeshLocation = VisualMesh->GetRelativeLocation();
		CollapseElapsedTime = 0.0f;

		if (DustNiagaraComp)
		{
			DustNiagaraComp->Activate(true);
		}
		if (DustNiagaraComp2)
		{
			DustNiagaraComp2->Activate(true);
		}

		GetWorld()->GetTimerManager().SetTimer(
			CollapseUpdateTimerHandle,
			this,
			&ALRCore::UpdateCollapseSequence,
			0.02f,
			true
		);
	}

	if (ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	if (DestructionCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), DestructionCameraShake, GetActorLocation(), 0.0f, 2000.0f);
	}

	SetLifeSpan(5.0f);
}