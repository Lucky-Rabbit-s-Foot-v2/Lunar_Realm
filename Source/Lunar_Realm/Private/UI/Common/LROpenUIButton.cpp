// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LROpenUIButton.h"

#include "Engine/GameInstance.h"

#include "Subsystems/UIManagerSubsystem.h"

void ULROpenUIButton::OnButtonClicked()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		if (TargetUIID != EUIID::NONE)
		{
			UIManager->OpenUIByID(TargetUIID);
		}
		else if (TargetUIClass)
		{
			UIManager->OpenUI(TargetUIClass);
		}
		else
		{
			LR_WARN(TEXT("No valid TargetUIID or TargetUIClass set for %s. Please set one of them in the widget blueprint."), *GetName());
		}
	}
	return;
}
