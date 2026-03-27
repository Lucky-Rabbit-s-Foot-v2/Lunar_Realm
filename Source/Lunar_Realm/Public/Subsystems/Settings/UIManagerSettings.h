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
 // (260327) KWB LRReadyStartWidget을 위한 EUIID 추가
 //=============================================================================

UENUM(BlueprintType)
enum class EUIID : uint8
{
	NONE,
	BACKGROUND,
	INTRO,
	TITLE,
	LOADING,
	LOBBY,
	SHOP,
	GACHA,
	PARTY,
	COLLECTION,
	DUTY,
	SETTING,
	PAUSE,
	GAMEOVER,
	GAMECLEAR,
	DAMAGEPOPUP,
	INGAME,
	OUTGAME,
	STAGE,
	READY,
	ENHANCE,
	READYSTART
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "UI Manager Settings"))
class LUNAR_REALM_API UUIManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UUIManagerSettings();

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI Routing")
	TMap<EUIID, TSoftClassPtr<ULRBaseWidget>> UIClassMap;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI Routing")
	TSubclassOf<ULRBaseWidget> DamageWidgetClass;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "UI Routing")
	TSubclassOf<ULRBaseWidget> TouchWidgetClass;
};
