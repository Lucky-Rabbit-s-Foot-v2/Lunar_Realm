// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Stage/LRStageGameState.h"

#include "Engine/GameInstance.h"
#include "Subsystems/StageManagerSubsystem.h"

void ALRStageGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ALRStageGameState::AddAether(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	CurrentAether += Amount;

	if (OnAetherChanged.IsBound())
	{
		OnAetherChanged.Execute(Amount);
	}
}
