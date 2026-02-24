// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPersistentWidget.generated.h"


//============================================================================
/**
 * UI Layer 의 배경이 될 위젯
 */
 //============================================================================
 // (260219) PJB 제작. 제반 사항 구현
 //============================================================================

UENUM(BlueprintType)
enum class EPersistentType : uint8
{
	INTRO UMETA(DisplayName = "Intro"),
	TRANSITION UMETA(DisplayName = "Transition"),
	LOBBY UMETA(DisplayName = "Lobby"),
	COLLECTION UMETA(DisplayName = "Collection"),
	PARTY UMETA(DisplayName = "Party"),
	GACHA UMETA(DisplayName = "Gacha"),
	STAGESELECTION UMETA(DisplayName = "StageSelection"),
	STAGE UMETA(DisplayName = "Stage"),
};

UCLASS()
class LUNAR_REALM_API ULRPersistentWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;
	virtual void OpenUI() override;

	class ULRPopupWidget* GetDefaultPopupWidget() { return DefaultPopupWidget; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI|Persistant")
	TSubclassOf<class ULRPopupWidget> DefaultPopupClass;

	UPROPERTY(VisibleAnywhere)
	class ULRPopupWidget* DefaultPopupWidget = nullptr;
};
