// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
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
class UMaterialInstanceDynamic;

UCLASS()
class LUNAR_REALM_API ULRSummonSlotWidget : public ULRBaseWidget
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

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UProgressBar> PB_Cooldown;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Cost;

	UPROPERTY(EditAnywhere, Category = "LR|UI")
	TObjectPtr<class UTexture2D> DefaultIcon;

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

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Cooldown;

	UPROPERTY()
	class UMaterialInstanceDynamic* CooldownMID;

protected:
	// 에디터에서 등급별 테두리 이미지를 세팅할 TMap
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|SummonSlot")
	TMap<ELRGrade, UTexture2D*> RarityBorderMap;

private:
	// 위젯에서 만든 테두리 이미지 바인딩
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Border;
};
