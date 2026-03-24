// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LREquipStatus.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULREquipStatus : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void RefreshUI() override;

	void SetEquipID(const FName& InID);
	void UpdateEquipData();
	void UpdateEquipStatus();

private:
	void UpdateLevel();
	void UpdateEXP();
	void UpdateSetType();
	void UpdateType();
	void UpdateATK();
	void UpdateDEF();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Level;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> EXP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> Bar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SetType;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Type;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ATK;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DEF;

private:
	FName ID;

	FEquipmentStaticData EquipmentStaticData;
	FEquipmentInstance EquipmentInstance;
};
