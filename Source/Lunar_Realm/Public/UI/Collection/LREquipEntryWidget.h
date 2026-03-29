// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LREntryWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LREquipEntryWidget.generated.h"

/**
 * 
 */

UCLASS()
class LUNAR_REALM_API ULREquipEntryWidget : public ULREntryWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshData() override;

	virtual void OnTileClicked() override;
};
