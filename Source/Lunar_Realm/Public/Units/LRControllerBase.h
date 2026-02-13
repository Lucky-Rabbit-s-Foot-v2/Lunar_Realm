// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "LRControllerBase.generated.h"

//============================================================================
/**
 * 플레이어 컨트롤러 베이스 클래스
 * - InGame / OutGame 공통 기능 구현
 */
 //============================================================================
 // (260127) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALRControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	template<typename T>
	T* OpenWidget(TSubclassOf<T> WidgetClass)
	{
		UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
		return UIManager->OpenUI<T>(WidgetClass);
	}
};
