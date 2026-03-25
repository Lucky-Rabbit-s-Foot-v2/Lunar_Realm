// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/LRStatusBarListWidget.h"
#include "UI/InGame/LRStatusBarSlot.h"
#include "Structures/Core/LRCore.h"
#include "Structures/Core/LRPlayerCore.h"
#include "Structures/Core/LREnemyCore.h"
#include "Units/Enemy/LREnemyBossCharacter.h"
#include "Structures/Spawner/LREnemySpawner.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "System/LoggingSystem.h"

void ULRStatusBarListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 코어 BeginPlay(속성 초기화)보다 먼저 실행되지 않도록 NextTick으로 지연
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ULRStatusBarListWidget::SetupSlotsDeferred);
}

void ULRStatusBarListWidget::NativeDestruct()
{
	if (ALREnemySpawner* Spawner = WeakSpawner.Get())
	{
		Spawner->OnBossSpawned.RemoveDynamic(this, &ULRStatusBarListWidget::OnBossSpawnedHandler);
	}

	Super::NativeDestruct();
}

void ULRStatusBarListWidget::SetupSlotsDeferred()
{
	SetupPlayerCoreSlot();
	SetupEnemySlot();
}

void ULRStatusBarListWidget::SetupPlayerCoreSlot()
{
	if (!SlotUI_0)
	{
		LR_WARN(TEXT("SetupPlayerCoreSlot: SlotUI_0 is null."));
		return;
	}

	ALRPlayerCore* PlayerCore = nullptr;
	for (TActorIterator<ALRPlayerCore> It(GetWorld()); It; ++It)
	{
		PlayerCore = *It;
		break;
	}

	if (!PlayerCore)
	{
		LR_WARN(TEXT("SetupPlayerCoreSlot: ALRPlayerCore not found in world."));
		SlotUI_0->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	WeakPlayerCore = PlayerCore;
	SlotUI_0->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SlotUI_0->SetupSlot(ESlotTargetType::PlayerCore, PlayerCore->GetAbilitySystemComponent(), PlayerCore->CoreIcon);
}

void ULRStatusBarListWidget::SetupEnemySlot()
{
	if (!SlotUI_1)
	{
		LR_WARN(TEXT("SetupEnemySlot: SlotUI_1 is null."));
		return;
	}

	GET_GI_SUBSYSTEM(UStageManagerSubsystem, StageSub);

	if (StageSub->IsBossStage())
	{
		// 보스는 OnGameStarted 이후 Spawner에 의해 동적 스폰됨.
		// 현재 스테이지 ID로 해당 Spawner를 찾아 OnBossSpawned 구독.
		SlotUI_1->SetVisibility(ESlateVisibility::Collapsed);

		const FName CurrentStageID = StageSub->GetCurrentStageID();
		ALREnemySpawner* Spawner = nullptr;
		for (TActorIterator<ALREnemySpawner> It(GetWorld()); It; ++It)
		{
			if (It->GetStageIDToActivate() == CurrentStageID)
			{
				Spawner = *It;
				break;
			}
		}

		if (!Spawner)
		{
			LR_ERROR(TEXT("SetupEnemySlot: ALREnemySpawner not found for stage [%s]"), *CurrentStageID.ToString());
			return;
		}

		WeakSpawner = Spawner;
		Spawner->OnBossSpawned.AddDynamic(this, &ULRStatusBarListWidget::OnBossSpawnedHandler);

		// 위젯 생성 지연으로 Broadcast를 이미 놓친 경우 — 보스가 이미 월드에 있으면 직접 바인딩
		for (TActorIterator<ALREnemyBossCharacter> It(GetWorld()); It; ++It)
		{
			Spawner->OnBossSpawned.RemoveDynamic(this, &ULRStatusBarListWidget::OnBossSpawnedHandler);
			BindBossSlot(*It);
			break;
		}
	}
	else
	{
		ALREnemyCore* EnemyCore = nullptr;
		for (TActorIterator<ALREnemyCore> It(GetWorld()); It; ++It)
		{
			EnemyCore = *It;
			break;
		}

		if (!EnemyCore)
		{
			LR_WARN(TEXT("SetupEnemySlot: ALREnemyCore not found in world."));
			SlotUI_1->SetVisibility(ESlateVisibility::Collapsed);
			return;
		}

		WeakEnemyCore = EnemyCore;
		SlotUI_1->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SlotUI_1->SetupSlot(ESlotTargetType::EnemyCore, EnemyCore->GetAbilitySystemComponent(), EnemyCore->CoreIcon);
	}
}

void ULRStatusBarListWidget::OnBossSpawnedHandler(ALREnemyBossCharacter* BossChar)
{
	if (ALREnemySpawner* Spawner = WeakSpawner.Get())
	{
		Spawner->OnBossSpawned.RemoveDynamic(this, &ULRStatusBarListWidget::OnBossSpawnedHandler);
	}

	BindBossSlot(BossChar);
}

void ULRStatusBarListWidget::BindBossSlot(ALREnemyBossCharacter* BossChar)
{
	if (!SlotUI_1 || !BossChar)
	{
		return;
	}

	UTexture2D* BossIcon = nullptr;
	GET_GI_SUBSYSTEM(UStageManagerSubsystem, StageSub);
	const FName BossEnemyID = StageSub->GetBossEnemyID();

	if (BossEnemyID.IsNone())
	{
		LR_WARN(TEXT("BindBossSlot: BossEnemyID is None — stage data BossEnemyID 필드를 확인하세요."));
	}
	else
	{
		GET_GI_SUBSYSTEM(UGameDataSubsystem, DataSub);
		const FEnemyStaticData& EnemyData = DataSub->GetEnemyStaticData(BossEnemyID);

		if (EnemyData.CharacterTexture.IsNull())
		{
			LR_WARN(TEXT("BindBossSlot: CharacterTexture null — DT_EnemyStaticData [%s] 확인하세요."), *BossEnemyID.ToString());
		}
		else
		{
			BossIcon = EnemyData.CharacterTexture.LoadSynchronous();
		}
	}

	WeakBossChar = BossChar;
	SlotUI_1->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SlotUI_1->SetupSlot(ESlotTargetType::BossCharacter, BossChar->GetAbilitySystemComponent(), BossIcon);
}
