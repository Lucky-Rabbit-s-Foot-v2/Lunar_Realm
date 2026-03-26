// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LREquipmentInfo.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULREquipmentInfo : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void RefreshUI() override;

	virtual void BindSubWidgets() override;
	virtual void RegisterSubWidgets() override;

	UFUNCTION(BlueprintCallable)
	void SetEquipIDCall(const FSelectedInfo& InInfo);

	void SetEquipID(const FName& InID);

	FName GetEquipID() const { return EquipID; }

	UFUNCTION(BlueprintCallable)
	void OnEnhanceButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREquipmentCard> EquipmentCard;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRSkillInfoWidget> SkillInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Enhance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULREquipStatus> EquipmentStatus;

	UPROPERTY(EditAnywhere, Category = "LR|Character Info")
	class UTexture2D* DefaultImage;

private:
	FName EquipID;
};
