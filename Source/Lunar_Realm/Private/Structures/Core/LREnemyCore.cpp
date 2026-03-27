// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Core/LREnemyCore.h"
#include "System/LoggingSystem.h"
#include "Core/Stage/LRStageGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/Attributes/LRCoreAttributeSet.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Units/Player/LRPlayerCharacter.h"

ALREnemyCore::ALREnemyCore()
{
	OwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Enemy.Structure.Core")));
}

void ALREnemyCore::BeginPlay()
{
	Super::BeginPlay();

	if (AttributeSet)
	{
		AttributeSet->InitHealth(5000.0f);
		AttributeSet->InitMaxHealth(5000.0f);
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(LRTags::Team_Enemy_Structure_Core);
	}

	LR_INFO(TEXT("적 코어 생성 및 체력 세팅 완료: %.1f"), AttributeSet->GetHealth());



}

void ALREnemyCore::OnCoreDestroyed()
{
	Super::OnCoreDestroyed();

	if (ALRPlayerCharacter* PlayerChar = Cast<ALRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PlayerChar->PlayVictoryVoice();
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
						WeakGM->OnGameClear();
					}
				}),
			5.0f,
			false
		);
	}

}
