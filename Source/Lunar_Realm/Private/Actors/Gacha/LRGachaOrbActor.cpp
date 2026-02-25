// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Gacha/LRGachaOrbActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ALRGachaOrbActor::ALRGachaOrbActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 이미시브 애니 필요할 때만 Tick 활성화

	// === Components ========================================================
	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	SetRootComponent(OrbMesh);

	IdleAura = CreateDefaultSubobject<UNiagaraComponent>(TEXT("IdleAura"));
	IdleAura->SetupAttachment(OrbMesh);
	IdleAura->bAutoActivate = false; // SetupOrb 이후 수동 활성화

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

	if (!bEmissiveAnimating)
	{
		return;
	}

	// 이미시브 0 → 최대값으로 선형 보간
	EmissiveAlpha += DeltaTime / EmissiveDuration;
	EmissiveAlpha = FMath::Clamp(EmissiveAlpha, 0.f, 1.f);

	if (DynMat)
	{
		const float MaxEmissive = GetEmissiveByRarity(CachedResult.Rarity);

		// Ease-Out 느낌: 초반 빠르게 상승, 후반 완만
		const float EasedAlpha = 1.f - FMath::Pow(1.f - EmissiveAlpha, 2.f);
		DynMat->SetScalarParameterValue(EmissiveParamName, EasedAlpha * MaxEmissive);
	}

	if (EmissiveAlpha >= 1.f)
	{
		OnEmissiveFinished();
	}
}

// ==========================================================================
// Public API
// ==========================================================================

void ALRGachaOrbActor::SetupOrb(const FLRGachaResult& InResult)
{
	CachedResult = InResult;

	// 등급에 따른 머티리얼 파라미터 세팅
	ApplyMaterialParams(InResult.Rarity);

	// Idle Aura 시작 (에셋이 세팅되어 있을 때만)
	if (IdleAura && IdleAuraSystem)
	{
		IdleAura->SetAsset(IdleAuraSystem);
		IdleAura->Activate(true);
	}
}

void ALRGachaOrbActor::PlayReveal()
{
	// 타이머 / 애니 중복 방지 초기화
	GetWorld()->GetTimerManager().ClearTimer(TimerSilhouette);

	bEmissiveAnimating = false;
	EmissiveAlpha = 0.f;
	SetActorTickEnabled(false);

	// 1단계: 실루엣 머티리얼로 교체 후, 일정 시간 유지
	PlaySilhouette_Internal();
}

void ALRGachaOrbActor::SetFocused(bool bFocused)
{
	// 포커스 여부에 따라 IdleAura 강도만 살짝 변경
	if (IdleAura)
	{
		IdleAura->SetFloatParameter(TEXT("User.Scale"), bFocused ? 1.5f : 1.0f);
	}

	// (필요 시 나중에 스케일 변경까지 추가 가능)
}

// ==========================================================================
// Internal Reveal Sequence
// ==========================================================================

void ALRGachaOrbActor::PlaySilhouette_Internal()
{
	if (!OrbMesh)
	{
		return;
	}

	// 실루엣 머티리얼로 교체
	if (SilhouetteMaterial)
	{
		OrbMesh->SetMaterial(0, SilhouetteMaterial);
	}

	// 실루엣 동안 IdleAura는 비활성화
	if (IdleAura)
	{
		IdleAura->Deactivate();
	}

	// SilhouetteDuration 경과 후 OnSilhouetteFinished 호출
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ALRGachaOrbActor::OnSilhouetteFinished);

	GetWorld()->GetTimerManager().SetTimer(
		TimerSilhouette,
		Delegate,
		SilhouetteDuration,
		false
	);
}

void ALRGachaOrbActor::OnSilhouetteFinished()
{
	if (!OrbMesh)
	{
		return;
	}

	// 원본 머티리얼 복원 + Dynamic Material Instance 재생성
	if (OrbMaterial)
	{
		// 원본 머티리얼로 복원
		OrbMesh->SetMaterial(0, OrbMaterial);

		DynMat = OrbMesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
		{
			// 색상은 즉시 적용, 이미시브는 0에서 시작 (Tick에서 애니메이션)
			DynMat->SetVectorParameterValue(ColorParamName, GetOrbColorByRarity(CachedResult.Rarity));
			DynMat->SetScalarParameterValue(EmissiveParamName, 0.f);
		}
	}

	// 이미시브 애니메이션 시작
	EmissiveAlpha = 0.f;
	bEmissiveAnimating = true;
	SetActorTickEnabled(true);

	// 리빌 버스트 이펙트
	if (RevealBurstSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			RevealBurstSystem,
			GetActorLocation()
		);
	}

	// 등급별 사운드 재생
	if (USoundBase* Sound = GetSoundByRarity(CachedResult.Rarity))
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}

void ALRGachaOrbActor::OnEmissiveFinished()
{
	bEmissiveAnimating = false;
	SetActorTickEnabled(false);

	// 이미시브를 최대값으로 고정
	if (DynMat)
	{
		DynMat->SetScalarParameterValue(
			EmissiveParamName,
			GetEmissiveByRarity(CachedResult.Rarity)
		);
	}

	// 리빌 완료를 BP에 알림 (카메라 연출 등)
	BP_OnRevealFinished(CachedResult.Rarity, CachedResult);
}

// ==========================================================================
// Utility
// ==========================================================================

FLinearColor ALRGachaOrbActor::GetOrbColorByRarity(ELRGachaRarity Rarity)
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return FLinearColor(0.8f, 0.8f, 0.8f, 1.f);
	case ELRGachaRarity::Elite:     return FLinearColor(0.1f, 0.4f, 1.0f, 1.f);
	case ELRGachaRarity::Unique:    return FLinearColor(0.9f, 0.1f, 0.1f, 1.f);
	case ELRGachaRarity::Epic:      return FLinearColor(0.5f, 0.1f, 0.9f, 1.f);
	case ELRGachaRarity::Legendary: return FLinearColor(1.0f, 0.75f, 0.0f, 1.f);
	default:                        return FLinearColor::White;
	}
}

float ALRGachaOrbActor::GetEmissiveByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return EmissiveCommon;
	case ELRGachaRarity::Elite:     return EmissiveElite;
	case ELRGachaRarity::Unique:    return EmissiveUnique;
	case ELRGachaRarity::Epic:      return EmissiveEpic;
	case ELRGachaRarity::Legendary: return EmissiveLegendary;
	default:                        return EmissiveCommon;
	}
}

USoundBase* ALRGachaOrbActor::GetSoundByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return SoundCommon;
	case ELRGachaRarity::Elite:     return SoundElite;
	case ELRGachaRarity::Unique:    return SoundUnique;
	case ELRGachaRarity::Epic:      return SoundEpic;
	case ELRGachaRarity::Legendary: return SoundLegendary;
	default:                        return SoundCommon;
	}
}

void ALRGachaOrbActor::ApplyMaterialParams(ELRGachaRarity Rarity)
{
	if (!OrbMesh || !OrbMaterial)
	{
		return;
	}

	DynMat = OrbMesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynMat)
	{
		return;
	}

	DynMat->SetVectorParameterValue(ColorParamName, GetOrbColorByRarity(Rarity));
	DynMat->SetScalarParameterValue(EmissiveParamName, GetEmissiveByRarity(Rarity));
}