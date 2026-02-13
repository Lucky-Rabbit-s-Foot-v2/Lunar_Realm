// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/LRLobbyHUD.h"

#include "Units/OutGame/LROutGameController.h"

void ALRLobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ALROutGameController* LRController = GetWorld()->GetFirstPlayerController<ALROutGameController>())
	{
		LRController->OpenLobbyWidget();
	}
}
