// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyCharacter.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"

ALREnemyCharacter::ALREnemyCharacter()
{
	// 스포너에서 ID 받아오는 함수

	// InitializeAttributes(/* 받은 ID 인자로 삽입 */);
}

void ALREnemyCharacter::BeginPlay()
{

}

void ALREnemyCharacter::InitializeAttributes() // TODO: ID 값 파라미터로 받는 걸로 수정 필요
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

	//// 실제로 적용할 것
	//const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);
	//int32 HP = EnemyData.Health;
	//int32 ATK = EnemyData.Attack;
	//int32 SPD = EnemyData.Speed;

	//AttributeSet->InitHealth(HP);
	//AttributeSet->InitAttack(ATK);
	//AttributeSet->InitSpeed(SPD);

}
