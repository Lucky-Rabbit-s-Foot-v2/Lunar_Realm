// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/LRProjectile.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameDataSubsystem.h"

ALRProjectile::ALRProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	// 충돌체 (루트)
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(100.f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	SphereComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// 이동 컴포넌트
	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileComp->InitialSpeed = 1000.f;
	ProjectileComp->MaxSpeed = 3000.f;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->ProjectileGravityScale = 0.f; // 기본은 중력 없음 (Arc에서 오버라이드)
	
	//Trail VFX 컴포넌트
	TrailVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVFX"));
	TrailVFXComponent->SetupAttachment(RootComponent);
	TrailVFXComponent->bAutoActivate = false;
}

void ALRProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentHit.AddDynamic(this, &ALRProjectile::OnHit);
}

void ALRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALRProjectile::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> Effect, float DamageValue)
{
	if (!TargetActor || !Effect)
	{
		LR_WARN(TEXT("Invalid TargetActor or GE Class"));
		return;
	}
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!ensureMsgf(TargetASC, TEXT("Invalid TargetASC")))
	{
		return;
	}
	
	UAbilitySystemComponent* SourceASC = InitData.InstigatorASC.Get();
	if (!ensureMsgf(SourceASC, TEXT("Invalid SourceASC")))
	{
		return;
	}
	
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(Effect, 1.f, ContextHandle);
	if (!SpecHandle.IsValid())
	{
		LR_WARN(TEXT("Invalid GE Handle for SkillObject"));
		return;
	}
	
	//데미지 수치 SetByCaller로 전달
	if (DamageValue > 0.f)
	{
		SpecHandle.Data->SetByCallerTagMagnitudes.Add(LRTags::Data_Damage, -DamageValue);
	}
	
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void ALRProjectile::PlaySpawnEffects()
{
	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("DataSubsystem Loading is Failed")))
	{
		return;
	}

	const FSkillResourceData& Resource = DataSys->GetSkillResourceData(InitData.ResourceID);

	// 스폰 VFX
	if (UNiagaraSystem* SpawnVFX = Resource.SpawnVFX.LoadSynchronous())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnVFX, GetActorLocation());
	}

	// 스폰 SFX
	if (USoundBase* SpawnSFX = Resource.SpawnSFX.LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSFX, GetActorLocation());
	}

	// 트레일 VFX + 컴포넌트 활성화
	if (UNiagaraSystem* TrailVFX = Resource.TrailVFX.LoadSynchronous())
	{
		TrailVFXComponent->SetAsset(TrailVFX);
		TrailVFXComponent->Activate(true);
	}
	
	LR_INFO(TEXT("Skill FX 발동!"));
}

void ALRProjectile::PlayImpactEffects()
{
	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("DataSubsystem Loading is Failed")))
	{
		return;
	}
	
	const FSkillResourceData& Resource = DataSys->GetSkillResourceData(InitData.ResourceID);

	// 충돌 VFX
	if (UNiagaraSystem* ImpactVFX = Resource.ImpactVFX.LoadSynchronous())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactVFX, GetActorLocation());
	}

	// 충돌 SFX
	if (USoundBase* ImpactSFX = Resource.ImpactSFX.LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSFX, GetActorLocation());
	}
}


void ALRProjectile::OnPoolActivate_Implementation()
{
	// TODO: 인터페이스 참고
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALRProjectile::OnPoolDeactivate_Implementation()
{
	// TODO: 인터페이스 참고
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer((LifeTimeTimerHandle));
	ProjectileComp->Velocity = FVector::ZeroVector;
	ProjectileComp->StopMovementImmediately();
}

void ALRProjectile::InitSkillObject(const FSkillObjectInitData& Initdata)
{
	InitData = Initdata;
	
	ProjectileComp->InitialSpeed = InitData.Speed;
	ProjectileComp->MaxSpeed = InitData.Speed;
	ProjectileComp->Velocity = GetActorForwardVector() * InitData.Speed;
	
	//Lifetime 타이머 시작
	GetWorldTimerManager().SetTimer(
		LifeTimeTimerHandle, this, &ALRProjectile::OnLifeTimeExpired, 
		InitData.Lifetime, false);
	
	// FX효과 재생
	PlaySpawnEffects();
	
	//자식 추가 초기화
	OnSkillObjectInitialized();
}

void ALRProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//자기자신 무시
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	
	//소환자 무시
	if (OtherActor == Cast<AActor>(GetInstigator()))
	{
		return;
	}
	
	// 충돌 이펙트 재생
	PlayImpactEffects();
	
	//HitType이 SINGLE일때만 베이스가 직접 데미지 처리
	//그 외에는 자식의 OnSkillObjectHIt에서 직접 처리
	//풀 복귀도 OnSkillObjectHit 처리 결과에 따라 부모/자식이 처리하는 로직 전환
	bool bHandledByChild = !OnSkillObjectHit(OtherActor, Hit);
	if (bHandledByChild)
	{
		return;
	}
	
	//데미지 GE적용
	if (InitData.DamageEffectClass)
	{
		ApplyEffectToTarget(OtherActor, InitData.DamageEffectClass, InitData.Damage);
	}
	
	//상태이상 GE적용
	if (InitData.StatusEffectClass)
	{
		ApplyEffectToTarget(OtherActor, InitData.StatusEffectClass, 0.f);
	}
	
	//풀 복귀
	OnPoolDeactivate_Implementation();
}

void ALRProjectile::OnLifeTimeExpired()
{
	OnSkillObjectExpired();
	OnPoolDeactivate_Implementation();
}

