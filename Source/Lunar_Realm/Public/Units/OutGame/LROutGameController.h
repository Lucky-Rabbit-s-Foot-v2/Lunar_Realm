// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "Data/LREnumType.h"
#include "Data/LRDataStructs.h"
#include "Sound/SoundBase.h"
#include "LROutGameController.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260210) BJM 제작. 아웃게임 컨트롤러 제작
 // (260213) PJB UI 관련 기능 추가
 // (260219) PJB 수정. HUD 제거, 소스 코드 이관.
 // (260325) PYI 로비(아웃게임 전체적용) BGM 추가
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectedChanged, FName, InID, ECollectionType, InType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonVisible, bool, InIsVisible);


UCLASS()
class LUNAR_REALM_API ALROutGameController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	ALROutGameController();

	virtual void OpenFirstWidget() override;

	UFUNCTION(BlueprintCallable)
	void OnSelectedEntryWidget(class ULREntryWidget* InWidget);

	void SetSelectedEntry(ULREntryWidget* InWidget);

	UFUNCTION(BlueprintCallable)
	void OnSelectedSlotWidget(class ULRSlotWidget* InWidget);

	void SetSelectedSlot(ULRSlotWidget* InWidget);

	UFUNCTION(BlueprintCallable)
	void OnSelectedSlotToggled(bool bIsSelected);

	void HandleMountAction(int32 InTargetIndex, ECollectionType InType, FName InID);
	void HandleSwapAction(int32 Slot1, int32 Slot2, ECollectionType InType);

	void OpenEnhancePage();
	void ReleasePartySlot();

	UFUNCTION()
	void SetIDAndType(FName InID, ECollectionType InType);
	void ResetSelectedData();
	void ResetWidgetEffect(ULRBaseWidget* Widget);

	UFUNCTION()
	void ResetSelectedWidget();

	UPROPERTY(BlueprintAssignable)
	FOnSelectedChanged OnSelectedChangedDel;

	UPROPERTY(BlueprintAssignable)
	FOnButtonVisible OnButtonVisibleDel;

	// 콘솔 명령어로 캐릭터나 장비가 확률대로 나오는지 확인하는 함수
	UFUNCTION(Exec)
	void GachaSim(const FString& BannerIdStr, int32 TotalPulls = 100000, int32 Seed = 12345);

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRGachaShopWidget> GachaShopWidgetClass;

	FName GetSelectedID() { return SelectedID; }
	ECollectionType GetSelectedType() { return SelectedType; }
	ULRBaseWidget* GetSelectedWidget() { return SelectedWidget.Get(); }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Sound")
	TObjectPtr<USoundBase> LobbyGachaBGMSound;

	TWeakObjectPtr<class ULRBaseWidget> SelectedWidget;
	TWeakObjectPtr<class ULRTileData> SelectedTileData;

	FName SelectedID = NAME_None;
	ECollectionType SelectedType = ECollectionType::NONE;
};
