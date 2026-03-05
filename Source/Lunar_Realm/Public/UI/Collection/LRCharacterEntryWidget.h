// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Common/LREntryWidget.h"
#include "LRCharacterEntryWidget.generated.h"


// =============================================================================
/**
 * 도감용 캐릭터 타일
 * - 클릭 시 캐릭터 선택 로직 추가
 */
 //=============================================================================
 // (260305) PJB 제작. 
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRCharacterEntryWidget : public ULREntryWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	UFUNCTION()
	void OnTileClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Selected;

};
