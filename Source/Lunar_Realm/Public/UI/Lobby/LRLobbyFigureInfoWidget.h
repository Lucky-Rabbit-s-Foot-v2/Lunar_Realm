// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRLobbyFigureInfoWidget.generated.h"


// =============================================================================
/**
 * 멤버 피규어 호버 시 등장할 위젯
 */
 //=============================================================================
 // (260212) PJB 제작.
 //=============================================================================

UCLASS()
class LUNAR_REALM_API ULRLobbyFigureInfoWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	void SetSlotIndex(int32 InSlotIndex);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Equipment;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	int32 SlotIndex;
};
