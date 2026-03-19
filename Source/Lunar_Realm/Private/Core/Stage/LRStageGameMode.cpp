// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Core/LRGameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Structures/Core/LREnemyCore.h"
#include "System/LoggingSystem.h"

#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

void ALRStageGameMode::OnGameOver()
{
	// 주의사항: UI 애니메이션도 멈춤
	// TODO : 방법 고민 필요. 일단은 일시정지 로직 재활용
	OnPauseGame();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::GAMEOVER);
}

void ALRStageGameMode::OnGameClear()
{
	// 주의사항: UI 애니메이션도 멈춤
	// TODO : 방법 고민 필요. 일단은 일시정지 로직 재활용
	OnPauseGame();

	// TODO: 보상 반영 등 코드 추가 필요하면 여기에 작성
	
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::GAMECLEAR);
}

void ALRStageGameMode::OnRestartGame()
{
	OnResetStage();
	LR_SCREEN_INFO(TEXT("Restart : Not implemented yet"));
}

void ALRStageGameMode::OnPauseGame()
{
	if (bIsGamePause)
	{
		return;
	}
	bIsGamePause = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void ALRStageGameMode::OnOpenPauseUI()
{
	OnPauseGame();

	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::PAUSE);
}

void ALRStageGameMode::OnResumeGame()
{
	if(!bIsGamePause)
	{
		return;
	}
	bIsGamePause = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void ALRStageGameMode::OnResetStage()
{
	LR_SCREEN_INFO(TEXT("Reset : Not implemented yet"));
}

void ALRStageGameMode::OnExitStage()
{
	OnResumeGame();

	ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance());
	GI->OpenNextLevelByName(ELevelName::LOBBY);
}

void ALRStageGameMode::OnStartNextStage()
{
	LR_SCREEN_INFO(TEXT("Start Next Stage : Not implemented yet"));
}

void ALRStageGameMode::BeginPlay()
{
	Super::BeginPlay();
	HideEnemyCoreIfBossStage();
}

void ALRStageGameMode::HideEnemyCoreIfBossStage()
{
	// TODO: 실제 빌드 시 사용할 버전
	//UGameInstance* GI = GetGameInstance();
	//UStageManagerSubsystem* StageMgr = GI ? GI->GetSubsystem<UStageManagerSubsystem>() : nullptr;
	//if (!GI || !StageMgr)
	//{
	//	LR_ERROR(TEXT("Not Valid GetGameInstance Or StageManagerSubsystem"));
	//	return;
	//}

	//if (!StageMgr->IsBossStage())
	//{
	//	// TEST
	//	LR_ERROR(TEXT("보스 스테이지 X"));
	//	return;
	//}

	// TEST: 테스트용 구현
	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		return;
	}

	FName CurrentStageID = NAME_None;
	if (const ULRGameInstance* LRGameInstance = Cast<ULRGameInstance>(GI))
	{
		CurrentStageID = LRGameInstance->GetCurrentStageID();
	}

	if (CurrentStageID == NAME_None)
	{
		LR_ERROR(TEXT("CurrentStageID is Null"));
		return;
	}
	// TEST: 테스트용 구현
	
	const FStageStaticData& StageData = DataSys->GetStageStaticData(CurrentStageID);
	if (!StageData.bIsBossStage)
	{
		// TEST
		LR_ERROR(TEXT("====== 보스 스테이지 아니라서 스킵됨 ======"));
		return;
	}

	ALREnemyCore* EnemyCore = Cast<ALREnemyCore>(UGameplayStatics::GetActorOfClass(GetWorld(), ALREnemyCore::StaticClass()));

	if (!EnemyCore)
	{
		LR_WARN(TEXT("[StageGameMode] Is BossStage, No EnemyCore Exist!"));
		return;
	}

	EnemyCore->SetActorHiddenInGame(true);
	EnemyCore->SetActorEnableCollision(false);

	if (UAbilitySystemComponent* ASC = EnemyCore->GetAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(LRTags::Team_Enemy_Structure_Core);
	}

	LR_INFO(TEXT("[StageGameMode] Is BossStage - EnemyCore Hidden and Tag Removed"));


}

AActor* ALRStageGameMode::ChoosePlayerStart_Implementation(AController* InPlayer)
{
	UGameInstance* GI = GetGameInstance();
	UStageManagerSubsystem* StageSys = GI ? GI->GetSubsystem<UStageManagerSubsystem>() : nullptr;

	if (StageSys)
	{
		const FStageStaticData* StageData = StageSys->GetCurrentStateData();

		if (StageData && !StageData->PlayerStartTag.IsNone())
		{
			FName TargetTag = StageData->PlayerStartTag;

			for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
			{
				APlayerStart* StartPoint = *It;

				if (StartPoint->PlayerStartTag == TargetTag)
				{
					LR_INFO(TEXT("[GameMode] %s 위치에서 플레이어 스폰"), *TargetTag.ToString());
					return StartPoint;
				}
			}

			LR_WARN(TEXT("[GameMode] %s 태그를 가진 PlayerStart가 맵에 없습니다"), *TargetTag.ToString());
		}
	}

	return Super::ChoosePlayerStart_Implementation(InPlayer);
}
