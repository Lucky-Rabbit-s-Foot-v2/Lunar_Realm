// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LROpenLevelButton.h"

#include "Core/LRGameInstance.h"

void ULROpenLevelButton::OnButtonClicked()
{
	if (LevelToOpen == ELevelName::NONE)
	{
		return;
	}

	if (ULRGameInstance* GI = GetGameInstance<ULRGameInstance>())
	{
		if(bShouldOpenImmediately)
		{
			GI->OpenNextLevelImmediately(LevelToOpen);
		}
		else
		{
			GI->OpenNextLevelByName(LevelToOpen);
		}
	}
}
