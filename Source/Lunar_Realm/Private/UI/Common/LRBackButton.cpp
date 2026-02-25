// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRBackButton.h"

#include "Engine/GameInstance.h"
#include "Components/Button.h"

#include "Subsystems/UIManagerSubsystem.h"

void ULRBackButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &ULRBackButton::OnBackButtonClicked);
	}
}

void ULRBackButton::OnBackButtonClicked()
{
	if (ParentWidget)
	{
		UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		UIManager->CloseUI(ParentWidget);
	}
}
