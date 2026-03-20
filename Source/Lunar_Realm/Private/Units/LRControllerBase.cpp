// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRControllerBase.h"

#include "System/LRCheatManager.h"

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