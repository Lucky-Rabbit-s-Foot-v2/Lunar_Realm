// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LREntryWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRCharacterEntryWidget.generated.h"


// =============================================================================
/**
 * 도감용 캐릭터 타일
 * - 클릭 시 캐릭터 선택 로직 추가
 */
 //=============================================================================
 // (260305) PJB 제작. 
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileClicked, FName, CharacaterID);

UCLASS()
class LUNAR_REALM_API ULRCharacterEntryWidget : public ULREntryWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void BindToController(class ALRControllerBase* Controller) override;

	virtual void RefreshData() override;

	UFUNCTION()
	void IsSelectedTile(const FSelectedInfo& InInfo);

	void SetSelected(bool bSelected);

	UFUNCTION()
	void OnTileClicked();

	UPROPERTY(BlueprintAssignable)
	FOnTileClicked OnTileClickedDel;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Selected;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Selected;

private:
	bool bIsSelected = false;
};
