// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/SkillActor/LRAoEActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameDataSubsystem.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Data/LRDataStructs.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Engine/TimerHandle.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"

// Sets default values
ALRAoEActor::ALRAoEActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	AoENiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AoENiagaraComp"));
	AoENiagaraComp->SetupAttachment(RootComponent);

	TickRate = 0.5f;

}

void ALRAoEActor::InitAoE(UAbilitySystemComponent* InInstigatorASC, TSubclassOf<UGameplayEffect> InDamageGE, FGameplayTag InHostileTag, FName InResourceID, float InRadius, float InDuration)
{
	InstigatorASC = InInstigatorASC;
	DamageEffectClass = InDamageGE;
	HostileTag = InHostileTag;

	CachedResourceID = InResourceID;

	CollisionSphere->SetSphereRadius(InRadius);
	SetLifeSpan(InDuration); 

	CollisionSphere->SetHiddenInGame(false);

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;

	if (DataSys && InResourceID != NAME_None)
	{
		const FSkillResourceData& ResourceData = DataSys->GetSkillResourceData(InResourceID);

		if (UNiagaraSystem* LoadedVFX = ResourceData.SpawnVFX.LoadSynchronous())
		{
			AoENiagaraComp->SetAsset(LoadedVFX);

			// 크기 조절
			float ScaleRatio = InRadius / 80.0f;
			AoENiagaraComp->SetRelativeScale3D(FVector(ScaleRatio, ScaleRatio, ScaleRatio));

			AoENiagaraComp->Activate(true);
		}

		if (USoundBase* LoadedSFX = ResourceData.SpawnSFX.LoadSynchronous())
		{
			UGameplayStatics::PlaySoundAtLocation(this, LoadedSFX, GetActorLocation());
		}
	}

	GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &ALRAoEActor::ApplyPeriodicDamage, TickRate, true);
}

void ALRAoEActor::ApplyPeriodicDamage()
{
	if (!InstigatorASC.IsValid() || !DamageEffectClass) return;

	UGameInstance* GI = GetWorld()->GetGameInstance();
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	const FSkillResourceData* ResourceData = (DataSys && CachedResourceID != NAME_None) ? &DataSys->GetSkillResourceData(CachedResourceID) : nullptr;

	TArray<AActor*> OverlappingActors;
	CollisionSphere->GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	for (AActor* HitActor : OverlappingActors)
	{
		if (HitActor == InstigatorASC->GetOwnerActor()) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (TargetASC && TargetASC->HasMatchingGameplayTag(HostileTag) && !TargetASC->HasMatchingGameplayTag(LRTags::State_Dead))
		{
			FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
			Context.AddInstigator(InstigatorASC->GetOwnerActor(), this);

			FGameplayEffectSpecHandle SpecHandle = InstigatorASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

			if (SpecHandle.IsValid())
			{
				InstigatorASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

				if (ResourceData)
				{
					if (UNiagaraSystem* LoadedImpactVFX = ResourceData->ImpactVFX.LoadSynchronous())
					{
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LoadedImpactVFX, HitActor->GetActorLocation());
					}

					if (USoundBase* LoadedImpactSFX = ResourceData->ImpactSFX.LoadSynchronous())
					{
						UGameplayStatics::PlaySoundAtLocation(this, LoadedImpactSFX, HitActor->GetActorLocation());
					}
				}

			}
		}
	}
}

