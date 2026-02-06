// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OutGame/LROutGameMode.h"

ALROutGameMode::ALROutGameMode()
{
}

void ALROutGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(nullptr);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}
