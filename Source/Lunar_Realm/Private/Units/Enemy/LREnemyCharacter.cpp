// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyCharacter.h"

#include "Engine/GameInstance.h"
#include "GAS/Attributes/LREnemyAttributeSet.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "System/LoggingSystem.h"

ALREnemyCharacter::ALREnemyCharacter()
{
	// 스포너에서 ID 받아오는 함수
	// enum 기반으로 처리되는 것 우선 wait


	// InitializeAttributes(/* 받은 ID 인자로 삽입 */);
}

void ALREnemyCharacter::OnDie()
{
	// 죽는 모션 재생
	// 컨트롤러 해제
	// 등등 죽었을 때 필요한 작업들 처리
	
	UPoolingSubsystem* PoolSys = GetWorld()->GetSubsystem<UPoolingSubsystem>();
	PoolSys->ReturnToPool(this);
}
void ALREnemyCharacter::BeginPlay()
{

}

void ALREnemyCharacter::InitializeAttributes(int32 EnemyID)
{
	if (!AttributeSet || !AbilitySystemComponent)
	{
		LR_ERROR(TEXT("AttributeSet or ASC is null"));
		return;
	}

	UGameInstance* GI = GetGameInstance();
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();

	if (!DataSys)
	{
		LR_ERROR(TEXT("GameDataSubsystem not found"));
		return;
	}

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);
	int32 HP = EnemyData.Health;
	int32 ATK = EnemyData.Attack;
	int32 SPD = EnemyData.Speed;

	AttributeSet->InitHealth(HP);
	AttributeSet->InitAttack(ATK);
	AttributeSet->InitSpeed(SPD);

	// LR_INFO(TEXT("Attributes initialized - HP: %.0f, ATK: %.0f, DEF: %.0f"), HP, ATK, SPD);
}
