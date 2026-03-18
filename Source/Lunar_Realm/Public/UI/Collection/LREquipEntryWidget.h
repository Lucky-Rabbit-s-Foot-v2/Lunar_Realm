// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LREntryWidget.h"
#include "LREquipEntryWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipTileClicked, FName, EquipID);

UCLASS()
class LUNAR_REALM_API ULREquipEntryWidget : public ULREntryWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshData() override;

	UFUNCTION()
	void OnTileClicked();

	UPROPERTY(BlueprintAssignable)
	FOnEquipTileClicked OnEquipTileClickedDel;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Selected;
};
