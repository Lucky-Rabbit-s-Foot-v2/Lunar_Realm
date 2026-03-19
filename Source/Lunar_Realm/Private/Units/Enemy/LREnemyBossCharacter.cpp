// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyBossCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/Stage/LRStageGameMode.h"
#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "System/LoggingSystem.h"
#include "Units/LRAIController.h"

ALREnemyBossCharacter::ALREnemyBossCharacter()
{
	AIControllerClass = ALREnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// TODO : 추후 Radius를 넓히고(멤버가 덜 바짝 붙는 용도), projectile 발사하는 스킬의 생성 트랜스폼을 수정하던지, 발사 각도를 수정하던지 스킬 먼저 만들고 수정
	// TEST : 값 테스트
	GetCapsuleComponent()->SetCapsuleSize(160.f, 200.f); // SetCapsuleSize(float InRadius, float InHalfHeight)

	UnitTag = LRTags::Team_Enemy_Character_Boss;
}

void ALREnemyBossCharacter::InitializeBossSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = SpeedPhase0;
}

void ALREnemyBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("[Boss] ASC를 찾을 수 없음"));
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(ULREnemyAttributeSet::GetHealthAttribute()).AddUObject(this, &ALREnemyBossCharacter::OnBossHealthChanged);
}

void ALREnemyBossCharacter::FinishDeathSequence()
{
	Super::FinishDeathSequence();

	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		StageGM->OnGameClear();
	}
	else
	{
		LR_ERROR(TEXT("[Boss] StageGameMode를 찾을 수 없음 - GameClear 호출 실패"));
	}
}

void ALREnemyBossCharacter::OnBossHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		LR_ERROR(TEXT("[Boss] ASC를 찾을 수 없음"));
		return;
	}

	const float MaxHealth = ASC->GetNumericAttribute(ULREnemyAttributeSet::GetMaxHealthAttribute());
	if (MaxHealth <= 0.0f)
	{
		return;
	}

	const float HealthPercent = Data.NewValue / MaxHealth;
	const int32 NewPhase = CalculatePhase(HealthPercent);

	if (NewPhase != CurrentPhase)
	{
		CurrentPhase = NewPhase;

		// TEST
		LR_ERROR(TEXT("=== [Boss] %d번째 페이즈로 전환 ==="), CurrentPhase);
		LR_ERROR(TEXT("=== [Boss] 현재 속도 : %f ==="), GetCharacterMovement()->MaxWalkSpeed);

		const float PhaseSpeedTable[] = { SpeedPhase0, SpeedPhase1, SpeedPhase2 };
		if (CurrentPhase < UE_ARRAY_COUNT(PhaseSpeedTable))
		{
			GetCharacterMovement()->MaxWalkSpeed = PhaseSpeedTable[CurrentPhase];
		}

		if (ALRAIController* AICtrl = Cast<ALRAIController>(GetController()))
		{
			if (UBlackboardComponent* BB = AICtrl->GetBlackboardComponent())
			{
				BB->SetValueAsInt(LRBBKeys::CurrentPhase, CurrentPhase);
				ActivateAuraVFX(CurrentPhase);
			}
			else
			{
				LR_ERROR(TEXT("[Boss] BB 찾을 수 없음"));
			}
		}
		else
		{
			LR_ERROR(TEXT("[Boss] 컨트롤러 찾을 수 없음"));
		}
	}
}

int32 ALREnemyBossCharacter::CalculatePhase(float HealthPercent) const
{
	for (int32 i = 0; i < PhaseThresholds.Num(); ++i)
	{
		if (HealthPercent > PhaseThresholds[i])
		{
			return i;
		}
	}
	return PhaseThresholds.Num();
}
