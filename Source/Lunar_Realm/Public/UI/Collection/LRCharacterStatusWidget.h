// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRCharacterStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCharacterStatusWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void RefreshUI() override;

	void SetCharacterID(const FName& InID);
	void UpdateCharacterData();
	void UpdateCharacterStatus();

private:
	FString GetClassNameByType(ELRClassType ClassType) const;

	void UpdateLevel();
	void UpdateEXP();
	void UpdateClass();
	void UpdateHP();
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
	TObjectPtr<class UTextBlock> Class;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ATK;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DEF;

private:
	FName ID;
	FCharacterStaticData CharacterStaticData;
	FCharacterInstance CharacterInstance;
};
