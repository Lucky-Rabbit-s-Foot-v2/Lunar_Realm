// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LREquipCollection.generated.h"


//============================================================================
/**
 * 장비 도감 위젯
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULREquipCollection : public ULRBaseWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> EquipTileView;

};
