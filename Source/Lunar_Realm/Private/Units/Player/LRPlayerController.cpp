// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"

ALRPlayerController::ALRPlayerController()
{
	PlayerCameraManagerClass = ALRPlayerCameraManager::StaticClass();
}
