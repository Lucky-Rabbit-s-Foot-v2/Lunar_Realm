// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Transition/LRTransitionGameMode.h"

#include "UI/Intro/LRLoadingPageWidget.h"
#include "Core/LRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Units/OutGame/LRTransitionController.h"
#include "Engine/LevelStreaming.h"
#include "Engine/AssetManager.h"

ALRTransitionGameMode::ALRTransitionGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALRTransitionGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsLoadingLevel && LoadingWidget)
	{
		CurrentFakeProgress = FMath::FInterpTo(CurrentFakeProgress, 0.9f, DeltaSeconds, 1.5f);

		LoadingWidget->SetLoadingProgress(CurrentFakeProgress);
	}
}

void ALRTransitionGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		LoadingWidget = Cast<ULRLoadingPageWidget>(UIManager->GetOrCreateWidgetByID(EUIID::LOADING));
		if (LoadingWidget)
		{
			LoadingWidget->SetLoadingProgress(0.0f);
			LR_INFO(TEXT("[로딩] LoadingWidget 캐싱 성공"));
		}
		else
		{
			LR_ERROR(TEXT("[로딩] LoadingWidget을 찾을 수 없음"));
		}
	}

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance()))
	{
		TargetLevelName = GI->GetNextLevelName();

		if (!TargetLevelName.IsNone())
		{
			PreloadAssetsAsync();
		}
	}
}

void ALRTransitionGameMode::OnLevelPreloadCompleted()
{
	bIsLoadingLevel = false;

	if (LoadingWidget)
	{
		LoadingWidget->FinishLoading();
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[this]()
		{
			// 최종적으로 맵으로 진입
			UGameplayStatics::OpenLevel(this, TargetLevelName);
		},
		LoadTime, // 1초 대기
		false
	);
}

void ALRTransitionGameMode::PreloadAssetsAsync()
{
	TArray<FSoftObjectPath> AssetsToLoad;

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		USaveGameSubsystem* SaveSys = GI->GetSubsystem<USaveGameSubsystem>();
		UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();

		// GameDataSubsystem 데이터 불러오기(캐릭터, 스킬 등)
		if (SaveSys && DataSys)
		{
			TArray<FName> AllPartyIDs = SaveSys->GetAllPartyCharactersIDs();

			for (FName CharID : AllPartyIDs)
			{
				if (CharID.IsNone()) continue;
				const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(CharID);
				if (!CharData.CharacterMesh.IsNull())
				{
					AssetsToLoad.AddUnique(CharData.CharacterMesh.ToSoftObjectPath());
				}
				if (!CharData.AnimBlueprintClass.IsNull())
				{
					AssetsToLoad.AddUnique(CharData.AnimBlueprintClass.ToSoftObjectPath());
				}
				for (FName SkillID : CharData.SkillIDs)
				{
					if (SkillID.IsNone()) continue;
					const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);
					if (SkillData.ResourceID.IsNone()) continue;
					const FSkillResourceData& ResourceData = DataSys->GetSkillResourceData(SkillData.ResourceID);
					if (!ResourceData.SpawnVFX.IsNull())
					{
						AssetsToLoad.AddUnique(ResourceData.SpawnVFX.ToSoftObjectPath());
					}
					if (!ResourceData.SpawnSFX.IsNull())
					{
						AssetsToLoad.AddUnique(ResourceData.SpawnSFX.ToSoftObjectPath());
					}
					if (!ResourceData.TrailVFX.IsNull())
					{
						AssetsToLoad.AddUnique(ResourceData.TrailVFX.ToSoftObjectPath());
					}
					if (!ResourceData.ImpactVFX.IsNull())
					{
						AssetsToLoad.AddUnique(ResourceData.ImpactVFX.ToSoftObjectPath());
					}
					if (!ResourceData.ImpactSFX.IsNull())
					{
						AssetsToLoad.AddUnique(ResourceData.ImpactSFX.ToSoftObjectPath());
					}
				}
			}
		}
	}

	if (AssetsToLoad.Num() > 0)
	{
		LR_INFO(TEXT("[로딩] 비동기 프리로드 시작 총 %d개의 애셋 대기 중"), AssetsToLoad.Num());
		bIsLoadingLevel = true;

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &ALRTransitionGameMode::StartLevelStreaming));
	}
	else
	{
		LR_WARN(TEXT("[로딩] 프리로드할 애셋이 없음. 바로 레벨 로딩 시작."));
		StartLevelStreaming();
	}
}

void ALRTransitionGameMode::StartLevelStreaming()
{
	bIsLoadingLevel = true;

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnLevelPreloadCompleted");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = FMath::Rand();

	UGameplayStatics::LoadStreamLevel(this, TargetLevelName, false, false, LatentInfo);
}
