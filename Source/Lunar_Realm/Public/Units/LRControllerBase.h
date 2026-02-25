// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "UI/Core/LRPersistentWidget.h"
#include "LRControllerBase.generated.h"

//============================================================================
/**
 * 플레이어 컨트롤러 베이스 클래스
 * - InGame / OutGame 공통 기능 구현
 */
 //============================================================================
 // (260127) PJB 제작.
 // (260219) PJB 수정. Persistent UI 제어 기능 추가.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALRControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void OpenPersistentWidget();

	UFUNCTION(BlueprintCallable)
	ULRBaseWidget* GetPersistentWidget();

	UFUNCTION(BlueprintCallable)
	void SetCurrentPersistentType(EPersistentType InPersistentType);

	template<typename T>
	T* OpenWidget(TSubclassOf<T> WidgetClass);

	UFUNCTION(BlueprintCallable)
	void CloseWidget(ULRBaseWidget* Widget);

	UFUNCTION(BlueprintCallable)
	virtual void OpenFirstWidget();

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TMap<EPersistentType, TSubclassOf<class ULRPersistentWidget>> PersistentWidgetClasses;

	UPROPERTY(VisibleAnywhere, Category = "LR|UI")
	EPersistentType CurrentPersistentType;
};

template<typename T>
T* ALRControllerBase::OpenWidget(TSubclassOf<T> WidgetClass)
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	return UIManager->OpenUI<T>(WidgetClass);
}