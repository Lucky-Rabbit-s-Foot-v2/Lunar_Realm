// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
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
	
	virtual void BindSubWidgets() override;
	virtual void RegisterSubWidgets() override;

	UFUNCTION()
	void SetIDAndType(FName InID, ECollectionType InType);

	void SetCharacterID(const FName& InID);

	FName GetCharacterID() const { return ID; }

	UFUNCTION(BlueprintCallable)
	void OnEnhanceButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterCard> CharacterCard;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRSkillInfoWidget> SkillInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Enhance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRCharacterStatusWidget> CharacterStatus;

	UPROPERTY(EditAnywhere, Category = "LR|Character Info")
	class UTexture2D* DefaultImage;

private:
	FName ID;
};
