// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRPartyPageWidget.generated.h"


//============================================================================
/**
 * 장비 도감 위젯
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRPartyPageWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
protected:
	virtual void RegisterSubWidgets() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotsWidget> PartySlot;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCollection> Collection;
};
