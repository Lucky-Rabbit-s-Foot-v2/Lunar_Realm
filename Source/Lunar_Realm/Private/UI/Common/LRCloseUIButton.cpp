// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRCloseUIButton.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRBaseWidget.h"

void ULRCloseUIButton::OnButtonClicked()
{
	if (!TargetWidget)
	{
		return;
	}

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->CloseUI(TargetWidget);
	}
}
