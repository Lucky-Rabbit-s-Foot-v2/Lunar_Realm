// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/LRPlayerState.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"

ALRPlayerState::ALRPlayerState()
{
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); 
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	
	AttributeSet = CreateDefaultSubobject<ULRPlayerAttributeSet>(TEXT("AttributeSet"));

	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

UAbilitySystemComponent* ALRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRPlayerState::InitializePlayerData()
{
	// TODO: 데이터 테이블 읽어서 스탯 초기화 로직 수행
	UE_LOG(LogTemp, Log, TEXT("Player State: Data Initialized!"));
}