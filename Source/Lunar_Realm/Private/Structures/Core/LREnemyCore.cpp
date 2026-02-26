// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LREnemyCore.h"
#include "System/LoggingSystem.h"
#include "Core/Stage/LRStageGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/Attributes/LRCoreAttributeSet.h"

ALREnemyCore::ALREnemyCore()
{
	OwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Enemy.Structure.Core")));
}

void ALREnemyCore::BeginPlay()
{
	Super::BeginPlay();

	if (AttributeSet)
	{
		AttributeSet->InitHealth(1000.0f);
		AttributeSet->InitMaxHealth(1000.0f);
	}

	LR_INFO(TEXT("적 코어 생성 및 체력 세팅 완료: %.1f"), AttributeSet->GetHealth());



}

void ALREnemyCore::OnCoreDestroyed()
{
	Super::OnCoreDestroyed();

	// TODO: GameMode에서 플레이어 승리 알림 호출
	LR_WARN(TEXT("적 코어 파괴 플레이어 승리"));
	
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		StageGM->OnGameOver();
	}
}
