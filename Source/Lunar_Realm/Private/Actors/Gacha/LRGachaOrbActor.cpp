// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Gacha/LRGachaOrbActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ALRGachaOrbActor::ALRGachaOrbActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// === Components ========================================================
	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	SetRootComponent(OrbMesh);

	IdleAura = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IdleAura"));
	IdleAura->SetupAttachment(OrbMesh);
	IdleAura->bAutoActivate = false;

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(OrbMesh);
	AudioComp->bAutoActivate = false;
}

void ALRGachaOrbActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALRGachaOrbActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ── 1) 중앙 이동 ───────────────────────────────────────────────
	if (bRevealMoving)
	{
		MoveAlpha += DeltaTime / FMath::Max(MoveToCenterDuration, KINDA_SMALL_NUMBER);
		MoveAlpha = FMath::Clamp(MoveAlpha, 0.f, 1.f);

		const float EasedAlpha = FMath::InterpEaseOut(0.f, 1.f, MoveAlpha, 2.0f);

		const FVector NewLocation = FMath::Lerp(CachedStartLocation, RevealTargetLocation, EasedAlpha);
		SetActorLocation(NewLocation);

		const FVector NewScale = FMath::Lerp(
			CachedStartScale,
			CachedStartScale * RevealScaleMultiplier,
			EasedAlpha
		);
		SetActorScale3D(NewScale);

		if (MoveAlpha >= 1.f)
		{
			OnMoveToCenterFinished();
		}
	}

	// ── 2) 이미시브 상승 ─────────────────────────────────────────
	if (bEmissiveAnimating)
	{
		EmissiveAlpha += DeltaTime / FMath::Max(EmissiveDuration, KINDA_SMALL_NUMBER);
		EmissiveAlpha = FMath::Clamp(EmissiveAlpha, 0.f, 1.f);

		if (DynMat)
		{
			const float MaxEmissive = GetEmissiveByRarity(CachedResult.Rarity);
			const float EasedAlpha = 1.f - FMath::Pow(1.f - EmissiveAlpha, 2.f);
			DynMat->SetScalarParameterValue(EmissiveParamName, EasedAlpha * MaxEmissive);
		}

		if (EmissiveAlpha >= 1.f)
		{
			OnEmissiveFinished();
		}
	}
}

// ==========================================================================
// Public API
// ==========================================================================

void ALRGachaOrbActor::SetupOrb(const FLRGachaResult& InResult)
{
	CachedResult = InResult;

	ApplyMaterialParams(InResult.Rarity);

	if (IdleAura && IdleAuraSystem)
	{
		IdleAura->SetAsset(IdleAuraSystem);
		IdleAura->Activate(true);
	}
}

void ALRGachaOrbActor::PlayRevealToCenter(const FVector& InTargetWorldLocation)
{
	// 중복 실행 방지
	if (bRevealMoving || bEmissiveAnimating || bRevealFinished)
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(TimerFinishReveal);

	CachedStartLocation = GetActorLocation();
	CachedStartScale = GetActorScale3D();
	RevealTargetLocation = InTargetWorldLocation;

	MoveAlpha = 0.f;
	EmissiveAlpha = 0.f;

	bRevealMoving = true;
	bEmissiveAnimating = false;
	bRevealFinished = false;

	// 클릭 직후 시작 사운드 재생
	if (USoundBase* StartSound = GetStartSoundByRarity(CachedResult.Rarity))
	{
		UGameplayStatics::PlaySoundAtLocation(this, StartSound, GetActorLocation());
	}

	// 중앙 이동 시작 시 Idle Aura는 정지
	if (IdleAura)
	{
		IdleAura->Deactivate();
	}

	if (OrbMesh)
	{
		UMaterialInterface* SelectedMaterial = GetMaterialByRarity(CachedResult.Rarity);
		if (SelectedMaterial)
		{
			DynMat = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
			if (DynMat)
			{
				OrbMesh->SetMaterial(0, DynMat);
				DynMat->SetVectorParameterValue(ColorParamName, GetOrbColorByRarity(CachedResult.Rarity));
				DynMat->SetScalarParameterValue(EmissiveParamName, 0.f);
			}
		}
	}

	SetActorTickEnabled(true);

	BP_OnRevealStarted(CachedResult);
}

void ALRGachaOrbActor::SetFocused(bool bFocused)
{
	if (IdleAura)
	{
		IdleAura->SetFloatParameter(TEXT("User.Scale"), bFocused ? 1.5f : 1.0f);
	}
}

// ==========================================================================
// Internal Reveal Sequence
// ==========================================================================

void ALRGachaOrbActor::OnMoveToCenterFinished()
{
	bRevealMoving = false;
	bEmissiveAnimating = true;

	// 중앙 도착 시 버스트/사운드 재생
	if (RevealBurstSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			RevealBurstSystem,
			GetActorLocation()
		);
	}

	if (USoundBase* Sound = GetSoundByRarity(CachedResult.Rarity))
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}

void ALRGachaOrbActor::OnEmissiveFinished()
{
	bEmissiveAnimating = false;

	if (DynMat)
	{
		DynMat->SetScalarParameterValue(
			EmissiveParamName,
			GetEmissiveByRarity(CachedResult.Rarity)
		);
	}

	// 약간 유지 후 실제 종료
	GetWorld()->GetTimerManager().SetTimer(
		TimerFinishReveal,
		this,
		&ALRGachaOrbActor::FinishReveal,
		HoldAtCenterDuration,
		false
	);
}

void ALRGachaOrbActor::FinishReveal()
{
	if (bRevealFinished)
	{
		return;
	}

	bRevealFinished = true;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	OnOrbRevealFinished.Broadcast(OrbIndex, CachedResult);
}

// ==========================================================================
// Utility
// ==========================================================================

FLinearColor ALRGachaOrbActor::GetOrbColorByRarity(ELRGachaRarity Rarity)
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:   return FLinearColor(0.65f, 0.65f, 0.65f, 1.f); // 회색
	case ELRGachaRarity::R:   return FLinearColor(0.55f, 1.00f, 0.35f, 1.f); // 연두색
	case ELRGachaRarity::SR:  return FLinearColor(0.15f, 0.45f, 1.00f, 1.f); // 파란색
	case ELRGachaRarity::SSR: return FLinearColor(0.60f, 0.20f, 0.90f, 1.f); // 보라색
	case ELRGachaRarity::UR:  return FLinearColor(1.00f, 0.78f, 0.10f, 1.f); // 황금색
	default:                  return FLinearColor::White;
	}
}

float ALRGachaOrbActor::GetEmissiveByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:   return EmissiveN;
	case ELRGachaRarity::R:   return EmissiveR;
	case ELRGachaRarity::SR:  return EmissiveSR;
	case ELRGachaRarity::SSR: return EmissiveSSR;
	case ELRGachaRarity::UR:  return EmissiveUR;
	default:                  return EmissiveN;
	}
}

USoundBase* ALRGachaOrbActor::GetSoundByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:   return SoundN;
	case ELRGachaRarity::R:   return SoundR;
	case ELRGachaRarity::SR:  return SoundSR;
	case ELRGachaRarity::SSR: return SoundSSR;
	case ELRGachaRarity::UR:  return SoundUR;
	default:                  return SoundN;
	}
}

USoundBase* ALRGachaOrbActor::GetStartSoundByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:   return StartSoundN;
	case ELRGachaRarity::R:   return StartSoundR;
	case ELRGachaRarity::SR:  return StartSoundSR;
	case ELRGachaRarity::SSR: return StartSoundSSR;
	case ELRGachaRarity::UR:  return StartSoundUR;
	default:                  return nullptr;
	}
}

void ALRGachaOrbActor::ApplyMaterialParams(ELRGachaRarity Rarity)
{
	if (!OrbMesh)
	{
		return;
	}

	UMaterialInterface* SelectedMaterial = GetMaterialByRarity(Rarity);
	if (!SelectedMaterial)
	{
		return;
	}

	DynMat = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
	if (!DynMat)
	{
		return;
	}

	OrbMesh->SetMaterial(0, DynMat);

	// 공통 파라미터를 쓰는 머티리얼이면 적용
	DynMat->SetVectorParameterValue(ColorParamName, GetOrbColorByRarity(Rarity));
	DynMat->SetScalarParameterValue(EmissiveParamName, GetEmissiveByRarity(Rarity));
}

UMaterialInterface* ALRGachaOrbActor::GetMaterialByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:
		return OrbMaterialN ? OrbMaterialN : OrbMaterialDefault;

	case ELRGachaRarity::R:
		return OrbMaterialR ? OrbMaterialR : OrbMaterialDefault;

	case ELRGachaRarity::SR:
		return OrbMaterialSR ? OrbMaterialSR : OrbMaterialDefault;

	case ELRGachaRarity::SSR:
		return OrbMaterialSSR ? OrbMaterialSSR : OrbMaterialDefault;

	case ELRGachaRarity::UR:
		return OrbMaterialUR ? OrbMaterialUR : OrbMaterialDefault;

	default:
		return OrbMaterialDefault;
	}
}