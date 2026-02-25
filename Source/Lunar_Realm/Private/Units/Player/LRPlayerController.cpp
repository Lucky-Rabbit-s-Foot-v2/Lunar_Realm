// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"
#include "Units/Player/LRPlayerState.h"
#include "GameFramework/TouchInterface.h"
#include "Units/Player/LRPlayerCharacter.h"

#include "UI/InGame/LRPlayerWidget.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "TimerManager.h"

ALRPlayerController::ALRPlayerController()
{
	PlayerCameraManagerClass = ALRPlayerCameraManager::StaticClass();
}

void ALRPlayerController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUI<ULRPlayerWidget>(PlayerWidget);
}

void ALRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (MobileTouchInterface)
		{
			ActivateTouchInterface(MobileTouchInterface);
			SetVirtualJoystickVisibility(true);
			LR_WARN(TEXT("터치 인터페이스 활성화 성공"));
		}
		else
		{
			LR_ERROR(TEXT("터치 인터페이스 활성화 실패"));
		}
	}
}

void ALRPlayerController::ToggleAutoMode()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->ToggleAutoMode();
	}
}

void ALRPlayerController::UsePotion()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->UsePotion();
	}
}

void ALRPlayerController::UseSkill1()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		ALRPlayerState* MyState = MyCharacter->GetPlayerState<ALRPlayerState>();
		if (MyState)
		{
			MyState->ActivateSkill1();
		}
	}
}

void ALRPlayerController::UseSkill2()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		ALRPlayerState* MyState = MyCharacter->GetPlayerState<ALRPlayerState>();
		if (MyState)
		{
			MyState->ActivateSkill2();
		}
	}
}

UAbilitySystemComponent* ALRPlayerController::GetAbilitySystemComponent()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	return MyCharacter ? MyCharacter->GetAbilitySystemComponent() : nullptr;
}

ULRPlayerWidget* ALRPlayerController::GetPlayerWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	return UIManager->GetOrCreateWidget(PlayerWidget);
}
