// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRCharacterStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCharacterStatusWidget : public ULRChildWidget
{
	GENERATED_BODY()

public:
	virtual void RefreshUI() override;

	void SetCharacterID(FName InID) { CharacterID = InID; }
	FName GetCharacterID() const { return CharacterID; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_HP;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_ATK;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_DEF;

private:
	FName CharacterID;
};
