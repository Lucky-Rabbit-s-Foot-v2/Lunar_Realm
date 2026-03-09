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

	// 선택: 씬 진입 시 플레이어 0 카메라로 자동 사용
	CameraComp->SetAutoActivate(true);
}

void ALRGachaOrbSceneActor::BeginPlay()
{
	Super::BeginPlay();

	// BP에서 조정된 최종 기본값을 저장
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

	const float CameraFrontAngleDeg = 180.f; // 카메라가 루트 뒤쪽에 있으므로 180 기준

	const int32 Count = CachedResults.Num();
	if (Count > 0)
	{
		const float StepAngle = 360.f / Count;

		// 인덱스 0이 카메라 정면에 오도록 초기 각도 설정
		CurrentCarouselAngle = CameraFrontAngleDeg - StepAngle * CenterOrbIndex;
		TargetCarouselAngle = CurrentCarouselAngle;
	}
	else
	{
		CurrentCarouselAngle = 0.f;
		TargetCarouselAngle = 0.f;
	}

	// 기존 구슬 정리
	for (AActor* Orb : OrbActors)
	{
		if (Orb)
		{
			Orb->Destroy();
		}
	}
	OrbActors.Empty();
	OrbStates.Empty();

	// 신규 구슬 스폰
	SpawnOrbs();

	// 달 이동 시작
	bMoonMoving = true;
	MoonMoveElapsed = 0.f;

	// 달 메쉬/머티리얼/위치 세팅
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

	// 달 아우라 나이아가라 세팅
	if (MoonAuraComponent && MoonAuraSystem)
	{
		MoonAuraComponent->SetAsset(MoonAuraSystem);
		MoonAuraComponent->Activate(true);
	}

	// BP 카메라 연출 시작
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
		// CurrentCarouselAngle를 포함한 초기 각도
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
				// 1) 인덱스 세팅(동기화 핵심)
				TypedOrb->SetOrbIndex(i);

				// 2) 결과 세팅
				if (CachedResults.IsValidIndex(i))
				{
					TypedOrb->SetupOrb(CachedResults[i]);
				}

				// 3) 리빌 완료 델리게이트 구독 (중복 방지)
				TypedOrb->OnOrbRevealFinished.RemoveDynamic(this, &ALRGachaOrbSceneActor::HandleOrbRevealFinished);
				TypedOrb->OnOrbRevealFinished.AddUniqueDynamic(this, &ALRGachaOrbSceneActor::HandleOrbRevealFinished);
			}
		}
	}

	// 초기 포커스는 0번
	SetCenterOrb(0);
}

// ==========================================================================
// Input : Swipe / Tap
// ==========================================================================

void ALRGachaOrbSceneActor::OnSwipeInput(float DeltaAngle)
{
	if (CachedResults.Num() <= 1 || OrbActors.Num() <= 1)
	{
		return;
	}

	// 모두 열렸으면 더 돌릴 필요 없음
	if (IsAllRevealed())
	{
		return;
	}

	const int32 Count = OrbActors.Num();
	const float StepAngle = 360.f / Count;

	// 스와이프 방향 → 인덱스 이동 방향 매핑
	// (혹시 UX가 반대로 느껴지면 부호만 뒤집으면 됨)
	const int32 IndexStep = (DeltaAngle > 0.f) ? -1 : 1;

	// 현재 센터 기준, 해당 방향으로 진행하면서
	// 처음 만나는 "미리빌" 구슬까지의 인덱스와 칸 수를 탐색
	int32 StepsToNext = 0;
	const int32 NextIndex = FindNextUnrevealedIndex(CenterOrbIndex, IndexStep, StepsToNext);
	if (NextIndex == INDEX_NONE || StepsToNext <= 0)
	{
		// 더 이상 해당 방향으로 열 수 있는 구슬 없음
		return;
	}

	// 센터 인덱스 갱신 (여기서 이미 Revealed는 스킵된 상태)
	SetCenterOrb(NextIndex);

	// 회전 각도는 "이번 스와이프로 몇 칸 이동했는지" 기준으로 상대 회전
	const float AngleDelta = -IndexStep * StepAngle * static_cast<float>(StepsToNext);

	// TargetCarouselAngle에 상대값 반영 → Tick에서 FInterpTo 보간
	TargetCarouselAngle += AngleDelta;
}

void ALRGachaOrbSceneActor::OnTapCenterOrb()
{
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

	// BP 스킵 카메라 연출
	BP_PlaySkipRevealAll(CachedResults);

	OnAllOrbsRevealed.Broadcast();
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

	// OrbActor에 PlayReveal 호출
	if (ALRGachaOrbActor* OrbActor = Cast<ALRGachaOrbActor>(OrbActors[Index]))
	{
		OrbActor->PlayReveal();
	}

	// "클릭됨" 이벤트는 1번만
	OnOrbClicked.Broadcast(Index);
}

void ALRGachaOrbSceneActor::SetCenterOrb(int32 NewIndex)
{
	const int32 Count = OrbActors.Num();
	if (Count <= 0)
	{
		return;
	}

	// 인덱스 보정 (0 ~ Count-1)
	int32 FixedIndex = NewIndex % Count;
	if (FixedIndex < 0)
	{
		FixedIndex += Count;
	}

	const bool bHasStates = (OrbStates.Num() == Count);

	// 지정 인덱스가 이미 리빌된 구슬이면,
	// 주변의 미리빌 구슬을 대신 센터로 지정
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

	// 결국 전부 Revealed이거나 기존 센터와 같으면 더 할 일 없음
	if (CenterOrbIndex == FixedIndex)
	{
		return;
	}

	// 이전 센터 포커스 해제
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

	// 새 센터 포커스 적용 (이미 리빌된 구슬이면 포커스 X)
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

	// 부드러운 회전 보간
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

		const float BaseAngle = StepAngle * i;
		const float FinalAngle = BaseAngle + CurrentCarouselAngle;

		// 위치만 업데이트 (스케일은 일단 고정)
		const FVector NewPos = GetActorLocation() + GetOrbPositionForAngle(FinalAngle);
		Orb->SetActorLocation(NewPos);

		// 필요 시 스케일 연출을 쓰고 싶으면 GetOrbScaleForAngle 사용
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

	// SmoothStep로 이징
	const float EasedProgress = FMath::SmoothStep(0.f, 1.f, Progress);

	const FVector MoonPos = FMath::Lerp(
		GetActorLocation() + MoonStartOffset,
		GetActorLocation() + MoonEndOffset,
		EasedProgress
	);

	// 달 메쉬/아우라 이동
	if (MoonMeshComp)
	{
		MoonMeshComp->SetWorldLocation(MoonPos);
	}

	if (MoonAuraComponent)
	{
		MoonAuraComponent->SetWorldLocation(MoonPos);
	}

	// BP에서 추가 연출
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

	// 기본 호흡: 아주 느린 Sine
	const float Wave = FMath::Sin(CameraBreathElapsed * CameraBreathSpeed * 2.f * PI);

	// 2차 웨이브를 아주 약하게 섞어서 기계적이지 않게
	const float WaveSecondary = FMath::Sin(CameraBreathElapsed * CameraBreathSpeed * 0.5f * 2.f * PI);

	FVector NewLocation = DefaultCameraBoomRelativeLocation;

	// 위아래 호흡
	NewLocation.Z += Wave * CameraBreathAmplitudeZ;

	// 아주 약한 측면 흔들림
	NewLocation.Y += WaveSecondary * CameraBreathAmplitudeY;

	CameraBoom->SetRelativeLocation(NewLocation);

	// 선택: FOV도 아주 미세하게
	if (bEnableCameraFOVBreathing && CameraComp)
	{
		const float FOVWave = (Wave * 0.5f) + 0.5f; // 0~1로 변환
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

	// 최소 1칸은 이동해야 하므로, 먼저 한 칸 이동 후 검사
	for (int32 Step = 0; Step < Count; ++Step)
	{
		Index = (Index + IndexStep + Count) % Count;
		++OutStepCount;

		if (OrbStates.IsValidIndex(Index) && OrbStates[Index] != ELROrbState::Revealed)
		{
			// 이 방향으로 이동하다가 처음 만나는 미리빌 구슬
			return Index;
		}
	}

	// 전부 Revealed인 경우
	OutStepCount = 0;
	return INDEX_NONE;
}

void ALRGachaOrbSceneActor::HandleOrbRevealFinished(int32 OrbIndex)
{
	if (!OrbStates.IsValidIndex(OrbIndex))
	{
		return;
	}

	// Revealing → Revealed 로 확정
	if (OrbStates[OrbIndex] != ELROrbState::Revealed)
	{
		OrbStates[OrbIndex] = ELROrbState::Revealed;
		RevealedCount++;

		// 전부 열렸는지 체크
		if (RevealedCount >= OrbActors.Num())
		{
			OnAllOrbsRevealed.Broadcast();
		}
	}
}