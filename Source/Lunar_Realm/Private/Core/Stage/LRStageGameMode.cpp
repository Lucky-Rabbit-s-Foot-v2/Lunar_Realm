// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Core/LRGameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Structures/Core/LREnemyCore.h"
#include "Structures/Core/LRPlayerCore.h"
#include "Structures/Core/LRCore.h"
#include "GAS/Attributes/LRCoreAttributeSet.h"

#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Components/AudioComponent.h"

#include "UI/InGame/LRGameClearPopupWidget.h"
#include "UI/InGame/LRReadyStartWidget.h"
#include "Subsystems/PoolingSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

#include "Units/Player/LRPlayerStart.h"
#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"

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
	UStageManagerSubsystem* StageMgr = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>();
	FName CurrentStageID = StageMgr->GetCurrentStageID();

	int32 StarMaskingBefore = StageMgr->GetStageClearedData(CurrentStageID).StarMasking;
	int32 StarMasking = CalculateStarMasking();

	const FStageRewardData& StageRewardData = StageMgr->GetCurrentStageRewardData();
	
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();

	if (StarMasking & 0b001)
	{
		SaveGameSubsystem->AddCurrency(ELRCurrencyType::Gold, StageRewardData.Gold);
	}
	
	if (!(0b010 & StarMaskingBefore))
	{
		if (0b010 & StarMasking)
		{
			SaveGameSubsystem->AddCurrency(ELRCurrencyType::CrescentTicket, StageRewardData.NormalTicket);
		}
	}

	if (!(0b100 & StarMaskingBefore))
	{
		if (0b100 & StarMasking)
		{
			SaveGameSubsystem->AddCurrency(ELRCurrencyType::FullMoonTicket, StageRewardData.EnhanceTicket);
		}
	}

	StarMasking |= StarMaskingBefore;
	StageMgr->ClearCurrentStage(StarMasking);
	OpenGameClearPopupWidget(StarMasking);
}

int32 ALRStageGameMode::CalculateStarMasking()
{
	int32 StarMasking = 0;
	if (IsStar1ConditionCheck())
	{
		StarMasking |= 0b001;
	}
	if (IsStar2ConditionCheck())
	{
		StarMasking |= 0b010;
	}
	if (IsStar3ConditionCheck())
	{
		StarMasking |= 0b100;
	}
	return StarMasking;
}

void ALRStageGameMode::OpenGameClearPopupWidget(int32 InStarMasking)
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	ULRGameClearPopupWidget* GameClearPopupWidget = Cast<ULRGameClearPopupWidget>(UIManager->OpenUIByID(EUIID::GAMECLEAR));
	if (GameClearPopupWidget)
	{
		CalculateStarMasking();

		UStageManagerSubsystem* StageMgr = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>();
		GameClearPopupWidget->SetStarMasking(InStarMasking);

		if (StageMgr->GetCurrentStageID() == FName("DESERT_05"))
		{
			GameClearPopupWidget->SetIsLastStage(true);
		}
		else
		{
			GameClearPopupWidget->SetIsLastStage(false);
		}
	}
}

void ALRStageGameMode::OnAnyCoreDestroyed(AActor* DestroyedCore)
{
	// TEST
	LR_INFO(TEXT("[GameMode] 코어 파괴 감지: %s — OnGameEnding Broadcast"), DestroyedCore ? *DestroyedCore->GetName() : TEXT("Unknown"));

	for (TActorIterator<ALRCore> It(GetWorld()); It; ++It)
	{
		It->OnCoreDestroyedEvent.RemoveDynamic(this, &ALRStageGameMode::OnAnyCoreDestroyed);
	}

	OnGameEnding.Broadcast();
}


void ALRStageGameMode::OnRestartGame()
{
	//OnResetStage();

	UStageManagerSubsystem* StageMgr = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>();
	FName CurrentStageID = StageMgr->GetCurrentStageID();
	
	ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance());
	GI->OpenNextStage(CurrentStageID);
	
	OnResumeGame();
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
	LR_SCREEN_INFO(TEXT("스테이지 초기화 중..."));
	LR_SCREEN_INFO(TEXT("초기화 로직 미완성..."));
	
	/**
	* TODO: 초기화 구현 필요
	* - 플레이어 위치 초기화
	* - 적 리스폰
	* - 풀링 시스템 초기화
	* - 코어 상태 초기화 (파괴된 경우 재생성)
	* - 기타 등등 초기화 필요한 요소들
	*/

	UStageManagerSubsystem* StageMgr = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>();
	FText CurrentStageName = StageMgr->GetCurrentStateData()->StageName;
	LR_SCREEN_INFO(TEXT("현재 스테이지: %s"), *CurrentStageName.ToString());

	OnResumeGame();

	// 임시 구현: 레벨 자체를 리셋하는 방식으로 초기화
	ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance());
	GI->OpenNextStage(GI->GetCurrentStageID());
}

void ALRStageGameMode::OnExitStage()
{
	OnResumeGame();

	ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance());
	GI->OpenNextLevelByName(ELevelName::LOBBY);
}

void ALRStageGameMode::OnStartNextStage()
{

	UStageManagerSubsystem* StageMgr = GetGameInstance()->GetSubsystem<UStageManagerSubsystem>();
	FName NextStageID = StageMgr->GetCurrentStateData()->NextStageID;

	ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance());
	GI->OpenNextStage(NextStageID);
	
	OnResumeGame();
}



void ALRStageGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnInitializeStage();
}

void ALRStageGameMode::OnInitializeStage()
{
	// 기존 BeginPlay에 있던 초기화 로직을 이쪽으로 이동
	CleanupUnusedCores();
	PlayBGM();

	PlayerDeathCount = 0;

	for (TActorIterator<ALRCore> It(GetWorld()); It; ++It)
	{
		It->OnCoreDestroyedEvent.AddDynamic(this, &ALRStageGameMode::OnAnyCoreDestroyed);
	}

	// Ready/Start UI 열기
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManager)
	{
		LR_ERROR(TEXT("[GameMode] UIManagerSubsystem not found"));
		StartGame();
		return;
	}

	ULRReadyStartWidget* ReadyStartWidget = Cast<ULRReadyStartWidget>(UIManager->OpenUIByID(EUIID::READYSTART));
	if (ReadyStartWidget)
	{
		ReadyStartWidget->OnReadySequenceFinished.AddDynamic(this, &ALRStageGameMode::OnReadySequenceFinished);
	}
	else
	{
		LR_WARN(TEXT("[GameMode] ReadyStartWidget not found — starting game immediately"));
		StartGame();
		return;
	}

	StartGame();

	// 입력 모드 금지
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->SetInputMode(FInputModeUIOnly());

		// TEST
		LR_INFO(TEXT("[GameMode] Input blocked — waiting for Ready/Start sequence"));
	}
}

void ALRStageGameMode::StartGame()
{
	// TEST
	LR_INFO(TEXT("[GameMode] Game Start! Broadcasting OnGameStarted delegate."));

	OnGameStarted.Broadcast();
}

void ALRStageGameMode::PlayBGM()
{
	if (BGMList.Num() > 0 && BGMList[0])
	{
		AudioComponent = UGameplayStatics::SpawnSound2D(this, BGMList[0], 1.f, 1.f, 0.f, nullptr, false, false);

		if (!ensureMsgf(AudioComponent, TEXT("Invalid AudioComp")))
		{
			return;
		}
		
		AudioComponent->OnAudioFinished.AddDynamic(this, &ALRStageGameMode::OnBGMFinished);
	}
}

void ALRStageGameMode::OnBGMFinished()
{
	if (!ensureMsgf(AudioComponent, TEXT("Invalid AudioComp")))
	{
		return;
	}
	
	if (BGMList.IsValidIndex(1) && BGMList[1])
	{
		AudioComponent->OnAudioFinished.RemoveAll(this); 
		AudioComponent->SetSound(BGMList[1]);
		AudioComponent->Play();

		AudioComponent->bStopWhenOwnerDestroyed = true;
	}
}

void ALRStageGameMode::OnReadySequenceFinished()
{
	// 입력 복원
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}

	// TEST
	LR_INFO(TEXT("[GameMode] Ready/Start sequence finished — input restored"));
}

void ALRStageGameMode::CleanupUnusedCores()
{
	UGameInstance* GI = GetGameInstance();
	UStageManagerSubsystem* StageSys = GI ? GI->GetSubsystem<UStageManagerSubsystem>() : nullptr;
	if (!StageSys) return;

	const FStageStaticData* StageData = StageSys->GetCurrentStateData();
	if (!StageData || StageData->PlayerStartTag.IsNone()) return;

	FName CurrentStageTag = StageData->PlayerStartTag;

	TArray<AActor*> AllCores;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALRCore::StaticClass(), AllCores);

	for (AActor* CoreActor : AllCores)
	{
		ALRCore* Core = Cast<ALRCore>(CoreActor);
		if (Core)
		{
			if (Core->StageTag != CurrentStageTag)
			{
				LR_INFO(TEXT("[GameMode] 사용하지 않는 코어 파괴됨: %s"), *Core->GetName());
				Core->Destroy();
			}
			else
			{
				LR_INFO(TEXT("[GameMode] 활성화된 코어 유지: %s"), *Core->GetName());
			}
		}
	}
}

void ALRStageGameMode::AddPlayerDeathCount()
{
	PlayerDeathCount++;
	LR_INFO(TEXT("[GameMode] 플레이어 사망 현재 데스 카운트: %d"), PlayerDeathCount);
}

bool ALRStageGameMode::IsStar1ConditionCheck()
{
	return true;
}

bool ALRStageGameMode::IsStar2ConditionCheck()
{
	return PlayerDeathCount <= 5;
}

bool ALRStageGameMode::IsStar3ConditionCheck()
{
	ALRPlayerCore* PlayerCore = Cast<ALRPlayerCore>(UGameplayStatics::GetActorOfClass(GetWorld(), ALRPlayerCore::StaticClass()));
	if (!PlayerCore || !PlayerCore->AttributeSet) return false;

	float CurrentHP = PlayerCore->AttributeSet->GetHealth();
	float MaxHP = PlayerCore->AttributeSet->GetMaxHealth();

	if (MaxHP <= 0.0f) return false;

	return (CurrentHP / MaxHP) >= 0.5f;

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
					if (ALRPlayerStart* LRStart = Cast<ALRPlayerStart>(StartPoint))
					{
						if (ALRPlayerController* PC = Cast<ALRPlayerController>(InPlayer))
						{
							if (ALRPlayerCameraManager* CamManager = Cast<ALRPlayerCameraManager>(PC->PlayerCameraManager))
							{
								CamManager->SetStageCameraBounds(LRStart->StageMinY, LRStart->StageMaxY);
								CamManager->ResetCameraInitialization();
								LR_INFO(TEXT("[GameMode] 카메라 한계선 세팅 완료: Min(%.1f), Max(%.1f)"), LRStart->StageMinY, LRStart->StageMaxY);
							}
						}
					}
					return StartPoint;
				}
			}

			LR_WARN(TEXT("[GameMode] %s 태그를 가진 PlayerStart가 맵에 없습니다"), *TargetTag.ToString());
		}
	}

	return Super::ChoosePlayerStart_Implementation(InPlayer);

}

