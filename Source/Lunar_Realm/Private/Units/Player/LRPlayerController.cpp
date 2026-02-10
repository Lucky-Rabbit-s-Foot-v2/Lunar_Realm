// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"
#include "GameFramework/TouchInterface.h"

ALRPlayerController::ALRPlayerController()
{
	PlayerCameraManagerClass = ALRPlayerCameraManager::StaticClass();
}

void ALRPlayerController::BeginPlay()
{
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
}
