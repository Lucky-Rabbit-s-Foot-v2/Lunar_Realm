// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRControllerBase.h"

void ALRControllerBase::OpenPersistentWidget()
{
	if (PersistentWidgetClasses.Contains(CurrentPersistentType))
	{
		TSubclassOf<ULRPersistentWidget> WidgetClass = PersistentWidgetClasses[CurrentPersistentType];
		UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		UIManager->OpenUI(WidgetClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PersistentWidgetClasses에 해당 PersistentType이 없습니다: %d"), static_cast<uint8>(CurrentPersistentType));
	}
}

UBaseWidget* ALRControllerBase::GetPersistentWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	return UIManager->GetOrCreateWidget(PersistentWidgetClasses[CurrentPersistentType]);
}

void ALRControllerBase::SetCurrentPersistentType(EPersistentType InPersistentType)
{
	if (PersistentWidgetClasses.Contains(InPersistentType))
	{
		CurrentPersistentType = InPersistentType;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PersistentWidgetClasses에 해당 PersistentType이 없습니다: %d"), static_cast<uint8>(InPersistentType));
	}
	OpenPersistentWidget();
}

void ALRControllerBase::CloseWidget(UBaseWidget* Widget)
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->CloseUI(Widget);
}
