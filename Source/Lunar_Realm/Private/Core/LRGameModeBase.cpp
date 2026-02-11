// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/LRGameModeBase.h"

void ALRGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(nullptr);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}
