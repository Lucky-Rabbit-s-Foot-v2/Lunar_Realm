// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/Core/LRBaseWidget.h"
#include "UIManagerSettings.generated.h"

// =============================================================================
/**
 * UI 매니저 관련 설정 클래스
 */
 //=============================================================================
 // (260225) PJB 제작. UIManagerSubsystem과 연동
 //=============================================================================

UENUM(BlueprintType)
enum class EUIID : uint8
{
	NONE,
	INTRO,
	TITLE,
	LOADING,
	LOBBY,
	SHOP,
	GACHA,
	PARTY,
	COLLECTION,
	DUTY,
	SETTING
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "UI Manager Settings"))
class LUNAR_REALM_API UUIManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UUIManagerSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI Routing")
	TMap<EUIID, TSoftClassPtr<ULRBaseWidget>> UIClassMap;
};
