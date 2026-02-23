// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"
#include "GameFramework/TouchInterface.h"
#include "Units/Player/LRPlayerCharacter.h"

#include "UI/Common/LRPersistentWidget.h"
#include "UI/Common/LRPopupWidget.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "TimerManager.h"

ALRPlayerController::ALRPlayerController()
{
	PlayerCameraManagerClass = ALRPlayerCameraManager::StaticClass();
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
			UE_LOG(LogTemp, Warning, TEXT("터치 인터페이스 활성화 성공"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("터치 인터페이스 활성화 실패"));
		}
	}

	SetCurrentPersistentType(EPersistentType::STAGE);

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

UAbilitySystemComponent* ALRPlayerController::GetAbilitySystemComponent()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	return MyCharacter ? MyCharacter->GetAbilitySystemComponent() : nullptr;
}
