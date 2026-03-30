// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LRPlayerCore.h"
#include "Core/Stage/LRStageGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "System/LoggingSystem.h"
#include "GAS/Attributes/LRCoreAttributeSet.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "Units/Player/LRPlayerController.h"
#include "UI/InGame/LRInGamePersistentWidget.h"

ALRPlayerCore::ALRPlayerCore()
{
	OwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Player.Structure.Core")));

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(RootComponent);
	SpawnArea->SetBoxExtent(FVector(100.0f, 300.0f, 100.0f));
	SpawnArea->SetRelativeLocation(FVector(300.0f, 0.0f, 0.0f));

	SpawnArea->SetCollisionProfileName(TEXT("NoCollision"));
	SpawnArea->SetHiddenInGame(true);

	MaxCoreHealth = 500.0f;
}

void ALRPlayerCore::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(LRTags::Team_Player_Structure_Core);
	}


	LR_INFO(TEXT("아군 코어 생성 및 체력 세팅 완료: %.1f"), AttributeSet->GetHealth());
}

FVector ALRPlayerCore::GetRandomSpawnLocation() const
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector RandomPointInLocal = UKismetMathLibrary::RandomPointInBoundingBox(FVector::ZeroVector, BoxExtent);

	return SpawnArea->GetComponentTransform().TransformPosition(RandomPointInLocal);
}

void ALRPlayerCore::OnCoreDestroyed()
{

	if (bIsDestroyed) return;

	if (ALRPlayerController* PC = Cast<ALRPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->CurrentGameSpeed = 1.0f;
		if (ULRInGamePersistentWidget* MyWidget = PC->GetPlayerWidget())
		{
			MyWidget->UpdateSpeedVisual(1.0f);
		}
	}
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	Super::OnCoreDestroyed();
	// TODO: GameMode에서 플레이어 패배 알림 호출
	LR_WARN(TEXT("플레이어 코어가 파괴되었습니다! 게임 오버(패배)!"));
	
	if (ALRPlayerCharacter* PlayerChar = Cast<ALRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PlayerChar->PlayDefeatVoice();
	}

	
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		FTimerHandle GameOverTimerHandle;

		TWeakObjectPtr<ALRStageGameMode> WeakGM = StageGM;

		GetWorld()->GetTimerManager().SetTimer(
			GameOverTimerHandle,
			FTimerDelegate::CreateLambda([WeakGM]()
				{
					if (WeakGM.IsValid())
					{
						WeakGM->OnGameOver();
					}
				}),
			5.0f,
			false 
		);
	}

}

