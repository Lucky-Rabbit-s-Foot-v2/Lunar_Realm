// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Gacha/LRGachaOrbSceneActor.h"
#include "Actors/Gacha/LRGachaOrbActor.h"

#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

ALRGachaOrbSceneActor::ALRGachaOrbSceneActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// === Root ==============================================================
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// === Moon ==============================================================

	MoonMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoonMesh"));
	MoonMeshComp->SetupAttachment(SceneRoot);

	MoonAuraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MoonAura"));
	MoonAuraComponent->SetupAttachment(GetRootComponent());
	MoonAuraComponent->bAutoActivate = false;

	// === Camera ============================================================

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(SceneRoot);
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CameraBoom);

	CameraComp->SetAutoActivate(true);
}

void ALRGachaOrbSceneActor::BeginPlay()
{
	Super::BeginPlay();

	if (CameraBoom)
	{
		DefaultCameraBoomRelativeLocation = CameraBoom->GetRelativeLocation();
	}

	if (CameraComp)
	{
		DefaultCameraFOV = CameraComp->FieldOfView;
	}
}

void ALRGachaOrbSceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCarouselTransforms(DeltaTime);
	UpdateMoonSequence(DeltaTime);
	UpdateCameraBreathing(DeltaTime);
}

// ==========================================================================
// Initialize & Spawn
// ==========================================================================

void ALRGachaOrbSceneActor::InitializeWithResults(const TArray<FLRGachaResult>& InResults)
{
	CachedResults = InResults;
	RevealedCount = 0;
	CenterOrbIndex = 0;
	bPresentationPlaying = false;

	const float CameraFrontAngleDeg = 180.f;

	const int32 Count = CachedResults.Num();
	if (Count > 0)
	{
		const float StepAngle = 360.f / Count;
		CurrentCarouselAngle = CameraFrontAngleDeg - StepAngle * CenterOrbIndex;
		TargetCarouselAngle = CurrentCarouselAngle;
	}
	else
	{
		CurrentCarouselAngle = 0.f;
		TargetCarouselAngle = 0.f;
	}

	for (AActor* Orb : OrbActors)
	{
		if (Orb)
		{
			Orb->Destroy();
		}
	}
	OrbActors.Empty();
	OrbStates.Empty();

	SpawnOrbs();

	bMoonMoving = true;
	MoonMoveElapsed = 0.f;

	if (MoonMeshComp)
	{
		if (MoonMesh)
		{
			MoonMeshComp->SetStaticMesh(MoonMesh);
		}

		if (MoonMaterial)
		{
			MoonMeshComp->SetMaterial(0, MoonMaterial);
		}

		MoonMeshComp->SetWorldLocation(GetActorLocation() + MoonStartOffset);
		MoonMeshComp->SetWorldScale3D(MoonMeshScale);
	}

	if (MoonAuraComponent && MoonAuraSystem)
	{
		MoonAuraComponent->SetAsset(MoonAuraSystem);
		MoonAuraComponent->Activate(true);
	}

	BP_PlayEnterSequence();
}

void ALRGachaOrbSceneActor::SpawnOrbs()
{
	if (!OrbActorClass || CachedResults.Num() == 0)
	{
		return;
	}

	const int32 Count = CachedResults.Num();
	OrbStates.SetNum(Count);

	const float StepAngle = 360.f / Count;

	for (int32 i = 0; i < Count; i++)
	{
		const float Angle = StepAngle * i + CurrentCarouselAngle;
		const FVector SpawnPos = GetActorLocation() + GetOrbPositionForAngle(Angle);
		const FTransform SpawnTr(FRotator::ZeroRotator, SpawnPos);

		AActor* Orb = GetWorld()->SpawnActor<AActor>(OrbActorClass, SpawnTr);
		if (Orb)
		{
			OrbActors.Add(Orb);
			OrbStates[i] = ELROrbState::Idle;

			if (ALRGachaOrbActor* TypedOrb = Cast<ALRGachaOrbActor>(Orb))
			{
				TypedOrb->SetOrbIndex(i);

				if (CachedResults.IsValidIndex(i))
				{
					TypedOrb->SetupOrb(CachedResults[i]);
				}

				TypedOrb->OnOrbRevealFinished.RemoveDynamic(this, &ALRGachaOrbSceneActor::HandleOrbRevealFinished);
				TypedOrb->OnOrbRevealFinished.AddUniqueDynamic(this, &ALRGachaOrbSceneActor::HandleOrbRevealFinished);
			}
		}
	}

	SetCenterOrb(0);
}

// ==========================================================================
// Input : Swipe / Tap
// ==========================================================================

void ALRGachaOrbSceneActor::OnSwipeInput(float DeltaAngle)
{
	if (bPresentationPlaying)
	{
		return;
	}

	if (CachedResults.Num() <= 1 || OrbActors.Num() <= 1)
	{
		return;
	}

	if (IsAllRevealed())
	{
		return;
	}

	const int32 Count = OrbActors.Num();
	const float StepAngle = 360.f / Count;

	const int32 IndexStep = (DeltaAngle > 0.f) ? -1 : 1;

	int32 StepsToNext = 0;
	const int32 NextIndex = FindNextUnrevealedIndex(CenterOrbIndex, IndexStep, StepsToNext);
	if (NextIndex == INDEX_NONE || StepsToNext <= 0)
	{
		return;
	}

	SetCenterOrb(NextIndex);

	const float AngleDelta = -IndexStep * StepAngle * static_cast<float>(StepsToNext);
	TargetCarouselAngle += AngleDelta;
}

void ALRGachaOrbSceneActor::OnTapCenterOrb()
{
	if (bPresentationPlaying)
	{
		return;
	}

	if (!CachedResults.IsValidIndex(CenterOrbIndex))
	{
		return;
	}

	if (OrbStates[CenterOrbIndex] != ELROrbState::Idle &&
		OrbStates[CenterOrbIndex] != ELROrbState::Focused)
	{
		return;
	}

	RevealOrb(CenterOrbIndex);
}

void ALRGachaOrbSceneActor::SkipAllReveal()
{
	for (int32 i = 0; i < OrbActors.Num(); i++)
	{
		if (OrbStates[i] != ELROrbState::Revealed)
		{
			OrbStates[i] = ELROrbState::Revealed;
		}
	}
	RevealedCount = OrbActors.Num();
	bPresentationPlaying = false;

	BP_PlaySkipRevealAll(CachedResults);

	OnAllOrbsRevealed.Broadcast();
}

void ALRGachaOrbSceneActor::NotifyPresentationClosed()
{
	bPresentationPlaying = false;
}

// ==========================================================================
// Reveal Logic
// ==========================================================================

void ALRGachaOrbSceneActor::RevealOrb(int32 Index)
{
	if (!OrbActors.IsValidIndex(Index))
	{
		return;
	}

	if (OrbStates[Index] == ELROrbState::Revealed || OrbStates[Index] == ELROrbState::Revealing)
	{
		return;
	}

	OrbStates[Index] = ELROrbState::Revealing;
	bPresentationPlaying = true;

	if (ALRGachaOrbActor* OrbActor = Cast<ALRGachaOrbActor>(OrbActors[Index]))
	{
		const FVector RevealTargetWorldLocation = GetActorLocation() + RevealCenterOffset;
		OrbActor->PlayRevealToCenter(RevealTargetWorldLocation);
	}
}

void ALRGachaOrbSceneActor::SetCenterOrb(int32 NewIndex)
{
	const int32 Count = OrbActors.Num();
	if (Count <= 0)
	{
		return;
	}

	int32 FixedIndex = NewIndex % Count;
	if (FixedIndex < 0)
	{
		FixedIndex += Count;
	}

	const bool bHasStates = (OrbStates.Num() == Count);

	if (bHasStates && OrbStates[FixedIndex] == ELROrbState::Revealed)
	{
		for (int32 Offset = 1; Offset < Count; ++Offset)
		{
			const int32 ForwardIndex = (FixedIndex + Offset) % Count;
			const int32 BackwardIndex = (FixedIndex - Offset + Count) % Count;

			if (OrbStates[ForwardIndex] != ELROrbState::Revealed)
			{
				FixedIndex = ForwardIndex;
				break;
			}

			if (OrbStates[BackwardIndex] != ELROrbState::Revealed)
			{
				FixedIndex = BackwardIndex;
				break;
			}
		}
	}

	if (CenterOrbIndex == FixedIndex)
	{
		return;
	}

	if (OrbActors.IsValidIndex(CenterOrbIndex) && bHasStates &&
		OrbStates[CenterOrbIndex] != ELROrbState::Revealed)
	{
		OrbStates[CenterOrbIndex] = ELROrbState::Idle;

		if (ALRGachaOrbActor* PrevOrb = Cast<ALRGachaOrbActor>(OrbActors[CenterOrbIndex]))
		{
			PrevOrb->SetFocused(false);
		}
	}

	CenterOrbIndex = FixedIndex;

	if (OrbActors.IsValidIndex(CenterOrbIndex) && bHasStates &&
		OrbStates[CenterOrbIndex] != ELROrbState::Revealed)
	{
		OrbStates[CenterOrbIndex] = ELROrbState::Focused;

		if (ALRGachaOrbActor* NewOrb = Cast<ALRGachaOrbActor>(OrbActors[CenterOrbIndex]))
		{
			NewOrb->SetFocused(true);
		}
	}
}

// ==========================================================================
// Tick Helpers
// ==========================================================================

void ALRGachaOrbSceneActor::UpdateCarouselTransforms(float DeltaTime)
{
	if (OrbActors.Num() == 0)
	{
		return;
	}

	const int32 Count = OrbActors.Num();
	const float StepAngle = 360.f / Count;

	CurrentCarouselAngle = FMath::FInterpTo(
		CurrentCarouselAngle,
		TargetCarouselAngle,
		DeltaTime,
		RotationSpeed
	);

	for (int32 i = 0; i < Count; ++i)
	{
		AActor* Orb = OrbActors[i];
		if (!Orb)
		{
			continue;
		}

		// 개별 리빌 중인 구슬은 자기 이동 애니메이션을 우선 사용
		if (OrbStates.IsValidIndex(i) && OrbStates[i] == ELROrbState::Revealing)
		{
			continue;
		}

		const float BaseAngle = StepAngle * i;
		const float FinalAngle = BaseAngle + CurrentCarouselAngle;

		const FVector NewPos = GetActorLocation() + GetOrbPositionForAngle(FinalAngle);
		Orb->SetActorLocation(NewPos);
		Orb->SetActorScale3D(FVector(1.0f));
	}
}

void ALRGachaOrbSceneActor::UpdateMoonSequence(float DeltaTime)
{
	if (!bMoonMoving)
	{
		return;
	}

	MoonMoveElapsed += DeltaTime;
	const float Progress = FMath::Clamp(MoonMoveElapsed / MoonMoveDuration, 0.f, 1.f);
	const float EasedProgress = FMath::SmoothStep(0.f, 1.f, Progress);

	const FVector MoonPos = FMath::Lerp(
		GetActorLocation() + MoonStartOffset,
		GetActorLocation() + MoonEndOffset,
		EasedProgress
	);

	if (MoonMeshComp)
	{
		MoonMeshComp->SetWorldLocation(MoonPos);
	}

	if (MoonAuraComponent)
	{
		MoonAuraComponent->SetWorldLocation(MoonPos);
	}

	BP_UpdateMoonPosition(EasedProgress);

	if (Progress >= 1.f)
	{
		bMoonMoving = false;
	}
}

void ALRGachaOrbSceneActor::UpdateCameraBreathing(float DeltaTime)
{
	if (!bEnableCameraBreathing || !CameraBoom)
	{
		return;
	}

	CameraBreathElapsed += DeltaTime;

	const float Wave = FMath::Sin(CameraBreathElapsed * CameraBreathSpeed * 2.f * PI);
	const float WaveSecondary = FMath::Sin(CameraBreathElapsed * CameraBreathSpeed * 0.5f * 2.f * PI);

	FVector NewLocation = DefaultCameraBoomRelativeLocation;
	NewLocation.Z += Wave * CameraBreathAmplitudeZ;
	NewLocation.Y += WaveSecondary * CameraBreathAmplitudeY;

	CameraBoom->SetRelativeLocation(NewLocation);

	if (bEnableCameraFOVBreathing && CameraComp)
	{
		const float FOVWave = (Wave * 0.5f) + 0.5f;
		const float NewFOV = DefaultCameraFOV + ((FOVWave - 0.5f) * 2.f * CameraBreathFOVAmplitude);
		CameraComp->SetFieldOfView(NewFOV);
	}
}

// ==========================================================================
// Utility
// ==========================================================================

FVector ALRGachaOrbSceneActor::GetOrbPositionForAngle(float AngleDeg) const
{
	const float Rad = FMath::DegreesToRadians(AngleDeg);
	return FVector(
		FMath::Cos(Rad) * CarouselRadius,
		FMath::Sin(Rad) * CarouselRadius,
		0.f
	);
}

int32 ALRGachaOrbSceneActor::FindNextUnrevealedIndex(int32 StartIndex, int32 IndexStep, int32& OutStepCount) const
{
	const int32 Count = OrbActors.Num();
	OutStepCount = 0;

	if (Count <= 0)
	{
		return INDEX_NONE;
	}

	int32 Index = StartIndex;

	for (int32 Step = 0; Step < Count; ++Step)
	{
		Index = (Index + IndexStep + Count) % Count;
		++OutStepCount;

		if (OrbStates.IsValidIndex(Index) && OrbStates[Index] != ELROrbState::Revealed)
		{
			return Index;
		}
	}

	OutStepCount = 0;
	return INDEX_NONE;
}

void ALRGachaOrbSceneActor::HandleOrbRevealFinished(int32 OrbIndex, const FLRGachaResult& Result)
{
	if (!OrbStates.IsValidIndex(OrbIndex))
	{
		return;
	}

	if (OrbStates[OrbIndex] != ELROrbState::Revealed)
	{
		OrbStates[OrbIndex] = ELROrbState::Revealed;
		RevealedCount++;
	}

	// 개별 구슬 연출이 끝났으므로 UI에 캐릭터/장비 리빌 표시 요청
	OnRevealPresentationRequested.Broadcast(OrbIndex, Result);

	if (RevealedCount >= OrbActors.Num())
	{
		OnAllOrbsRevealed.Broadcast();
	}
}

void ALRGachaOrbSceneActor::StopAllOrbSounds()
{
	for (AActor* Orb : OrbActors)
	{
		if (ALRGachaOrbActor* TypedOrb = Cast<ALRGachaOrbActor>(Orb))
		{
			TypedOrb->StopAllOrbSounds();
		}
	}
}