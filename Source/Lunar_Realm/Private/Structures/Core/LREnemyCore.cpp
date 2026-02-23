// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LREnemyCore.h"
#include "System/LoggingSystem.h"

ALREnemyCore::ALREnemyCore()
{
	OwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Enemy.Structure.Core")));
}

void ALREnemyCore::BeginPlay()
{
	Super::BeginPlay();

	LR_INFO(TEXT("적 코어 생성 완료"));
}

void ALREnemyCore::OnCoreDestroyed()
{
	Super::OnCoreDestroyed();

	// TODO: GameMode에서 플레이어 승리 알림 호출
	LR_WARN(TEXT("적 코어 파괴 플레이어 승리"));
}
