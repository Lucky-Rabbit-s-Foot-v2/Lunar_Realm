// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRControllerBase.h"

#include "System/LRCheatManager.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Components/InputComponent.h"

ALRControllerBase::ALRControllerBase()
{
	CheatClass = ULRCheatManager::StaticClass();
}

void ALRControllerBase::BeginPlay()
{
	Super::BeginPlay();

	OpenFirstWidget();
}

void ALRControllerBase::OpenFirstWidget()
{
	// 자식 클래스에서 구현하도록 비워둠
}

void ALRControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindTouch(IE_Pressed, this, &ALRControllerBase::OnTouchBegan);
	}
}

void ALRControllerBase::OnTouchBegan(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		FVector2D ScreenPos(Location.X, Location.Y);
		UIManager->ShowTouchEffect(ScreenPos);
	}
}


