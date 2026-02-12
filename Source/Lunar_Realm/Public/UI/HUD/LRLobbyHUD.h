// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseHUD.h"
#include "LRLobbyHUD.generated.h"

// =============================================================================
/**
 * OutGame 전용 HUD
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ALRLobbyHUD : public ABaseHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void OpenLobbyWidget();
	
	UFUNCTION(BlueprintCallable)
	void OpenShopWidget();
	
	UFUNCTION(BlueprintCallable)
	void OpenShopWidgetByCurrency();
	
	UFUNCTION(BlueprintCallable)
	void OpenGachaShopWidget();

	UFUNCTION(BlueprintCallable)
	void OpenCollectionWidget();
	
	UFUNCTION(BlueprintCallable)
	void OpenPartyWidget();
	
	UFUNCTION(BlueprintCallable)
	void OpenChapterWidget();
	
	UFUNCTION(BlueprintCallable)
	void OpenStageWidget();
	
	UFUNCTION(BlueprintCallable)
	void OpenSettingsWidget();

	UFUNCTION(BlueprintCallable)
	void OpenFigureInfo(FName CharacterID);
	
	UFUNCTION(BlueprintCallable)
	void CloseFigureInfo();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRLobbyWidget> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class UBaseWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRGachaShopWidget> GachaShopWidgetClass;

};
