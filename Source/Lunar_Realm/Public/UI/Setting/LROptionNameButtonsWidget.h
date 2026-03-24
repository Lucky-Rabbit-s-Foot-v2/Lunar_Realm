// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Subsystems/Option/OptionManagerSubsystem.h"
#include "LROptionNameButtonsWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOptionButtonClicked, ESettingType, InType, int32, Number);

UCLASS()
class LUNAR_REALM_API ULROptionNameButtonsWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UPROPERTY(BlueprintAssignable)
	FOnOptionButtonClicked OnOptionButtonClickedDel;

	UFUNCTION()
	int32 GetCurrentNumber() const { return CurrentNumber; }


	UFUNCTION(BlueprintCallable)
	void OnLowButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnMediumButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OnHighButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Low;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Medium;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Setting")
	ESettingType SettingType = ESettingType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Button")
	FText OptionName = FText();

private:
	int32 CurrentNumber = 0;
};
