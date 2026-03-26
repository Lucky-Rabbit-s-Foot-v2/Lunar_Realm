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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedChanged, const FSelectedInfo&, InSelectedInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotSelected, bool, InIsSelected);

UCLASS()
class LUNAR_REALM_API ALROutGameController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	ALROutGameController();

	virtual void OpenFirstWidget() override;

	UFUNCTION(BlueprintCallable)
	void SetSelectedCharacterID(FName InID);

	UFUNCTION(BlueprintCallable)
	void SetSelectedEquipmentID(FName InID);

	FName GetSelectedCharacterID();
	FName GetSelectedEquipmentID();

	const FSelectedInfo& GetSelectedInfo() const { return SelectedInfo; }
	void SetSelectedInfo(const FSelectedInfo& InInfo) { SelectedInfo = InInfo; }
	void ResetSelectedInfo() { 
		SelectedInfo = FSelectedInfo(); 
		OnSlotSelectedDel.Broadcast(false);
		OnSelectedChangedDel.Broadcast(SelectedInfo);
	}

	UPROPERTY(BlueprintAssignable)
	FOnSelectedChanged OnSelectedChangedDel;

	UPROPERTY(BlueprintAssignable)
	FOnSlotSelected OnSlotSelectedDel;

	UFUNCTION()
	void RequestUpdateSelectedInfo(const FSelectedInfo& InInfo);

	bool IsSlotSelected(const FSelectedInfo& InInfo);
	bool IsTaskSelected(const FSelectedInfo& InInfo);
	bool IsCellSelected(const FSelectedInfo& InInfo);

	void HandleMountAction(const FSelectedInfo& Target, const FSelectedInfo& Source);
	void HandleSwapAction(int32 Slot1, int32 Slot2);

	void OpenEnhancePage();

	UFUNCTION()
	void OnPartyPageOpened();

	UFUNCTION()
	void OnPartyPageClosed();

	// 콘솔 명령어로 캐릭터나 장비가 활률대로 나오는지 확인하는 함수
	UFUNCTION(Exec)
	void GachaSim(const FString& BannerIdStr, int32 TotalPulls = 100000, int32 Seed = 12345);

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRGachaShopWidget> GachaShopWidgetClass;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Sound")
	TObjectPtr<USoundBase> LobbyGachaBGMSound;

	FSelectedInfo SelectedInfo;
};
