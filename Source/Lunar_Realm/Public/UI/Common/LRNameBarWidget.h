// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "LRNameBarWidget.generated.h"

// =============================================================================
/**
 * 이름과 바만 있는 간단한 UI 위젯
 */
 //=============================================================================
 // (260219) PJB 제작. 제반 사항 구현.
 // =============================================================================

UCLASS()
class LUNAR_REALM_API ULRNameBarWidget : public ULRChildWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetName(const FText& Name);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> Bar_Progress;
};
