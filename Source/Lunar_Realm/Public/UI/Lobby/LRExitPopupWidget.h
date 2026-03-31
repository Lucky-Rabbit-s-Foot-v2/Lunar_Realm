// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRExitPopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRExitPopupWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;


	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnBackButtonClicked();


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Exit;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRButtonWidget> Btn_Back;

};
