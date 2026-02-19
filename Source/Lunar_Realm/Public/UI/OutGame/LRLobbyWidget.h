// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LRPopupWidget.h"
#include "LRLobbyWidget.generated.h"

// =============================================================================
/**
 * 로비 UI 위젯
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRLobbyWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
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

};
