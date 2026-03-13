// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Subsystems/Option/OptionManagerSubsystem.h"
#include "LRNameBarWidget.generated.h"

// =============================================================================
/**
 * 이름과 바만 있는 간단한 UI 위젯
 */
 //=============================================================================
 // (260219) PJB 제작. 제반 사항 구현.
 // =============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProgressBarChanged, ESettingType, InType, int32, Value);

UCLASS()
class LUNAR_REALM_API ULRNameBarWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void SetName(const FText& Name);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnProgressBarChanged OnProgressBarChangedDel;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	void UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> Bar_Progress;

	UPROPERTY(EditAnywhere, Category = "Settings")
	ESettingType SettingType;

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 MinValue = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 MaxValue = 0.0f;

private:
	bool bIsDragging = false;

	int32 CurrentValue = 0;
};
