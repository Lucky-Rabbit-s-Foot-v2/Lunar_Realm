// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MapSettings.generated.h"

// =============================================================================
/**
 * 맵 에셋 관련 설정 클래스
 */
 //=============================================================================
 // (260226) PJB 제작. GameInstance에서 맵 전환 시 활용 예정
 //=============================================================================

UENUM(BlueprintType)
enum class ELevelName : uint8
{
	NONE			UMETA(DisplayName = "None"),
	TRANSITION		UMETA(DisplayName = "Transition"),
	INTRO			UMETA(DisplayName = "Intro"),
	LOBBY			UMETA(DisplayName = "Lobby"),
	STAGE			UMETA(DisplayName = "Stage")
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Map Settings"))
class LUNAR_REALM_API UMapSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UMapSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Level Routing")
	TMap<ELevelName, TSoftObjectPtr<UWorld>> LevelMap;
};
