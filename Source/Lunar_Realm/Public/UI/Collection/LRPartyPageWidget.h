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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPartyPageOpened);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPartyPageClosed);

UCLASS()
class LUNAR_REALM_API ULRPartyPageWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void RegisterSubWidgets() override;

	virtual void OpenUI() override;
	virtual void CloseUI() override;

	UPROPERTY(BlueprintAssignable)
	FOnPartyPageOpened OnPartyPageOpenedDel;

	UPROPERTY(BlueprintAssignable)
	FOnPartyPageClosed OnPartyPageClosedDel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRPartySlotsWidget> PartySlot;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCollection> Collection;
};
