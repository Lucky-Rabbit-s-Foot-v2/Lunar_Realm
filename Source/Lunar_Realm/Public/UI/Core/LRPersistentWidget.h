// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRPersistentWidget.generated.h"


//============================================================================
/**
 * 게임 중 항상 떠 있는 UI
 * - 체력 바, 스킬 버튼 등
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
	ULRPersistentWidget(const FObjectInitializer& ObjectInitializer);

	virtual void OpenUI() override;
};
