// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "Data/Gacha/LRGachaTypes.h"
#include "UI/Gacha/LRGachaRevealWidget.h"
#include "LRGachaShopWidget.generated.h"

class UButton;
class UTextBlock;
class ULRGachaSubsystem;
/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGachaShopWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 현재 탭 상태 판단용 배너ID(항상 FullMoon 기본값 유지: Hero_FullMoon / Equip_FullMoon)
	UPROPERTY(BlueprintReadOnly, Category = "LR|Gacha")
	FName CurrentBannerID;

	// 초기 배너 지정(블루프린트에서 세팅)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha")
	FName DefaultHeroBannerID = TEXT("Hero_FullMoon");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Gacha")
	FName DefaultEquipBannerID = TEXT("Equip_FullMoon");

	// 리빌 위젯 띄우기
	void ShowRevealWidget(FName InBannerID, const TArray<FLRGachaResult>& InResults);

protected:
	// 위젯 바인딩
	UPROPERTY(meta = (BindWidget)) UButton* ButtonHeroTab;
	UPROPERTY(meta = (BindWidget)) UButton* ButtonEquipTab;

	UPROPERTY(meta = (BindWidget)) UButton* ButtonCrescentDraw1;
	UPROPERTY(meta = (BindWidget)) UButton* ButtonCrescentDraw10;
	UPROPERTY(meta = (BindWidget)) UButton* ButtonFullMoonDraw1;
	UPROPERTY(meta = (BindWidget)) UButton* ButtonFullMoonDraw10;

	// 천장 표시용(보름달 배너)
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* TextPity;

	// 재화 표시(상단 3칸)
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* TextGold;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* TextCrescent;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* TextFullMoon;

private:
	UPROPERTY()
	ULRGachaSubsystem* GachaSys = nullptr;

	// 리빌 팝업 클래스(블루프린트에서 지정)
	UPROPERTY(EditAnywhere, Category = "LR|Gacha|UI")
	TSubclassOf<ULRGachaRevealWidget> RevealWidgetClass;

	// Draw 버튼 클릭에서 트랜잭션 시작 + 리빌 띄우기
	void TryBeginDrawAndOpenReveal(FName BannerID, int32 Count);

private:
	UFUNCTION()
	void OnClickHeroTab();

	UFUNCTION()
	void OnClickEquipTab();

	UFUNCTION() void OnClickCrescentDraw1();
	UFUNCTION() void OnClickCrescentDraw10();
	UFUNCTION() void OnClickFullMoonDraw1();
	UFUNCTION() void OnClickFullMoonDraw10();


	UFUNCTION()
	void HandleCurrencyChanged(FGameplayTag Tag, int32 NewValue);

	UFUNCTION()
	void HandlePityChanged(FName BannerID, int32 NewValue);
	
	void RefreshCurrencyTexts();
	void RefreshPityText();

	FName MakeBannerIDForTicket(bool bFullMoon) const;
	bool IsHeroTabSelected() const;

};
