// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LROutGameController.generated.h"

/**
 * 
 */

 //=============================================================================
 // (260210) BJM 제작. 아웃게임 컨트롤러 제작
 // (260213) PJB UI 관련 기능 추가
 // (260219) PJB 수정. HUD 제거, 소스 코드 이관.
 //=============================================================================

UENUM(BlueprintType)
enum class ESelectedType : uint8
{
	CHARACTER = 0,
	EQUIPMENT,
	NONE,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectedChanged, ESelectedType, SelectedType, FName, NewCharacterID);

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

	UFUNCTION(BlueprintCallable)
	void SetSelectedIDNone();

	FName GetSelectedCharacterID();
	FName GetSelectedEquipmentID();

	ESelectedType GetSelectedType() { return SelectedType; }
	FName GetSelectedID() { return SelectedID; }

	UPROPERTY(BlueprintAssignable)
	FOnSelectedChanged OnSelectedChangedDel;

	// 콘솔 명령어로 캐릭터나 장비가 활률대로 나오는지 확인하는 함수
	UFUNCTION(Exec)
	void GachaSim(const FString& BannerIdStr, int32 TotalPulls = 100000, int32 Seed = 12345);

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRGachaShopWidget> GachaShopWidgetClass;

protected:
	ESelectedType SelectedType = ESelectedType::NONE;
	FName SelectedID = NAME_None;
};
