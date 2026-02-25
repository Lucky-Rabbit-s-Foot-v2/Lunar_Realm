// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "Data/LRDataStructs.h"
#include "LRSummonSlotWidget.generated.h"

/**
 * - BindWidget을 통해 WBP 디자인 요소와 C++ 로직 연결
 * - 쿨타임 및 비용 실시간 갱신 담당
 */

 // =============================================================================
 // (260216) BJM 제작. 소환 슬롯 위젯
 // =============================================================================

class UButton;
class UImage;
class UProgressBar;
class UTextBlock;
class ULRSummonComponent;

UCLASS()
class LUNAR_REALM_API ULRSummonSlotWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UFUNCTION(BlueprintCallable, Category = "Summon")
	void InitSlot(int32 InSlotIndex, FName InUnitID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Summon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Cooldown;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Cost;

protected:
	int32 SlotIndex = -1;
	FName UnitID;
	float SummonCost = 0.0f;
	float TotalCooldown = 0.0f;
	float CurrentCooldown = 0.0f;

	UPROPERTY()
	TObjectPtr<ULRSummonComponent> SummonComp;

private:
	UFUNCTION()
	void OnSummonedEvent(int32 InSlotIndex, float InCooldownTime);

	UFUNCTION()
	void OnSummonButtonClicked();


	void UpdateCooldownState(float InDeltaTime);
	void UpdateButtonState();
	void SetSlotVisuals(const FCharacterStaticData* Data);

};
