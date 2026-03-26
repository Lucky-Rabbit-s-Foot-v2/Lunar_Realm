// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Transition/LRTransitionGameMode.h"

#include "UI/Intro/LRLoadingPageWidget.h"
#include "Core/LRGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"
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

		if (SaveSys && DataSys)
		{
			GatherCharacterAssets(SaveSys, DataSys, AssetsToLoad);

			const UMapSettings* MapSettings = GetDefault<UMapSettings>();
			FName StageMapName = NAME_None;

			if (MapSettings)
			{
				if (const TSoftObjectPtr<UWorld>* StageMapPtr = MapSettings->LevelMap.Find(ELevelName::STAGE))
				{
					StageMapName = FName(StageMapPtr->GetLongPackageFName());
				}
			}

			if (TargetLevelName == StageMapName)
			{
				GatherEnemyAssets(GI, DataSys, AssetsToLoad);
			}
		}
	}

	// 수집된 에셋 로딩 시작
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

void ALRTransitionGameMode::GatherCharacterAssets(USaveGameSubsystem* InSaveSys, UGameDataSubsystem* InDataSys, TArray<FSoftObjectPath>& OutAssetsToLoad)
{
	TArray<FName> AllPartyIDs = InSaveSys->GetAllPartyCharactersIDs();

	for (FName CharID : AllPartyIDs)
	{
		if (CharID.IsNone()) continue;

		const FCharacterStaticData& CharData = InDataSys->GetCharacterStaticData(CharID);
		if (!CharData.CharacterMesh.IsNull())
		{
			OutAssetsToLoad.AddUnique(CharData.CharacterMesh.ToSoftObjectPath());
		}
		if (!CharData.AnimBlueprintClass.IsNull())
		{
			OutAssetsToLoad.AddUnique(CharData.AnimBlueprintClass.ToSoftObjectPath());
		}

		for (FName SkillID : CharData.SkillIDs)
		{
			if (SkillID.IsNone()) continue;

			const FSkillStaticData& SkillData = InDataSys->GetSkillStaticData(SkillID);
			if (SkillData.ResourceID.IsNone()) continue;

			const FSkillResourceData& ResourceData = InDataSys->GetSkillResourceData(SkillData.ResourceID);

			if (!ResourceData.SpawnVFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.SpawnVFX.ToSoftObjectPath());
			if (!ResourceData.SpawnSFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.SpawnSFX.ToSoftObjectPath());
			if (!ResourceData.TrailVFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.TrailVFX.ToSoftObjectPath());
			if (!ResourceData.ImpactVFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.ImpactVFX.ToSoftObjectPath());
			if (!ResourceData.ImpactSFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.ImpactSFX.ToSoftObjectPath());
		}
	}
}

void ALRTransitionGameMode::GatherEnemyAssets(UGameInstance* InGI, UGameDataSubsystem* InDataSys, TArray<FSoftObjectPath>& OutAssetsToLoad)
{
	UStageManagerSubsystem* StageMgr = InGI->GetSubsystem<UStageManagerSubsystem>();
	if (!StageMgr) return;

	TArray<FName> EnemyIDsToLoad;
	const FStageStaticData* StageData = StageMgr->GetCurrentStateData();

	if (StageData)
	{
		for (const FName& EnemyID : StageData->SpawnEnemyIDs)
		{
			EnemyIDsToLoad.AddUnique(EnemyID);
		}
		if (!StageData->BossEnemyID.IsNone())
		{
			EnemyIDsToLoad.AddUnique(StageData->BossEnemyID);
		}
	}

	if (EnemyIDsToLoad.IsEmpty())
	{
		LR_WARN(TEXT("[TransitionGameMode - Enemy] : 스테이지에 등장할 에너미가 없습니다."));
		return;
	}

	for (const FName& EnemyID : EnemyIDsToLoad)
	{
		if (EnemyID.IsNone()) continue;

		// 비주얼/애니메이션 에셋
		const FEnemyStaticData& EnemyData = InDataSys->GetEnemyStaticData(EnemyID);
		if (!EnemyData.EnemyMesh.IsNull())			OutAssetsToLoad.AddUnique(EnemyData.EnemyMesh.ToSoftObjectPath());
		if (!EnemyData.CharacterTexture.IsNull())	OutAssetsToLoad.AddUnique(EnemyData.CharacterTexture.ToSoftObjectPath());
		if (!EnemyData.AnimBlueprintClass.IsNull()) OutAssetsToLoad.AddUnique(EnemyData.AnimBlueprintClass.ToSoftObjectPath());
		if (!EnemyData.AttackedMontage.IsNull())	OutAssetsToLoad.AddUnique(EnemyData.AttackedMontage.ToSoftObjectPath());
		if (!EnemyData.DeathMontage.IsNull())		OutAssetsToLoad.AddUnique(EnemyData.DeathMontage.ToSoftObjectPath());

		for (const TSoftObjectPtr<UAnimMontage>& SoftMontage : EnemyData.AttackMontages)
		{
			if (!SoftMontage.IsNull()) OutAssetsToLoad.AddUnique(SoftMontage.ToSoftObjectPath());
		}
		for (const TSoftObjectPtr<UNiagaraSystem>& SoftVFX : EnemyData.AuraVFXList)
		{
			if (!SoftVFX.IsNull()) OutAssetsToLoad.AddUnique(SoftVFX.ToSoftObjectPath());
		}

		// 스킬 리소스 에셋
		for (const FName& SkillID : EnemyData.SkillIDs)
		{
			if (SkillID.IsNone()) continue;

			const FSkillStaticData& SkillData = InDataSys->GetSkillStaticData(SkillID);
			if (SkillData.ResourceID.IsNone()) continue;

			const FSkillResourceData& ResourceData = InDataSys->GetSkillResourceData(SkillData.ResourceID);

			if (!ResourceData.SpawnVFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.SpawnVFX.ToSoftObjectPath());
			if (!ResourceData.SpawnSFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.SpawnSFX.ToSoftObjectPath());
			if (!ResourceData.TrailVFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.TrailVFX.ToSoftObjectPath());
			if (!ResourceData.ImpactVFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.ImpactVFX.ToSoftObjectPath());
			if (!ResourceData.ImpactSFX.IsNull())	OutAssetsToLoad.AddUnique(ResourceData.ImpactSFX.ToSoftObjectPath());
		}

		// 사운드 에셋
		const FEnemySoundData& SoundData = InDataSys->GetEnemySoundData(EnemyID);

		if (!SoundData.DeathSound.IsNull())  OutAssetsToLoad.AddUnique(SoundData.DeathSound.ToSoftObjectPath());
		if (!SoundData.IntroVoice.IsNull())  OutAssetsToLoad.AddUnique(SoundData.IntroVoice.ToSoftObjectPath());


		for (const TSoftObjectPtr<USoundBase>& SoftSFX : SoundData.AttackSounds)
		{
			if (!SoftSFX.IsNull()) OutAssetsToLoad.AddUnique(SoftSFX.ToSoftObjectPath());
		}
		for (const TSoftObjectPtr<USoundBase>& SoftSFX : SoundData.HitSound)
		{
			if (!SoftSFX.IsNull()) OutAssetsToLoad.AddUnique(SoftSFX.ToSoftObjectPath());
		}
		for (const TSoftObjectPtr<USoundBase>& SoftSFX : SoundData.FootstepSounds)
		{
			if (!SoftSFX.IsNull()) OutAssetsToLoad.AddUnique(SoftSFX.ToSoftObjectPath());
		}
	}
}
