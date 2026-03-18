// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRLobbyPageWidget.generated.h"

// =============================================================================
/**
 * 로비 UI 위젯
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRLobbyPageWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	virtual void RegisterSubWidgets() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRLobbyFigureWidget> MainCharacter;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRLobbyFigureWidget> Member1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRLobbyFigureWidget> Member2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRLobbyFigureWidget> Member3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRLobbyFigureWidget> Member4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPageSelectorWidget> PageSelector;
};
