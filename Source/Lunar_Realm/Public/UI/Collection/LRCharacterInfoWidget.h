// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRCharacterInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRCharacterInfoWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void RefreshUI() override;
	
	virtual void RegisterSubWidgets() override;

	UFUNCTION(BlueprintCallable)
	void SetCharacterID(FName InID);

	FName GetCharacterID() const { return CharacterID; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Grade;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Main;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equip1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equip2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equip3;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterStatusWidget> CharacterStatus;

private:
	FName CharacterID;
	FName Equip1ID;
	FName Equip2ID;
	FName Equip3ID;
};
