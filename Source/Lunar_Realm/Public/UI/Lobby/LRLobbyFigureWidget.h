// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LRDataStructs.h"
#include "Data/LREnumType.h"
#include "LRLobbyFigureWidget.generated.h"


// =============================================================================
/**
 * 로비에 띄울 피규어 위젯
 */
 //=============================================================================
 // (260212) PJB 제작. 
 // (260212) PJB 주의사항 : Unhover 전에 Hover 이벤트가 발생할 수 있음. 
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFigureClicked, int32, InSlotIndex);

UCLASS()
class LUNAR_REALM_API ULRLobbyFigureWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshUI() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureClicked OnFigureClickedDel;
	
	UFUNCTION()
	void OnFigurePressed();

	UFUNCTION()
	void OnFigureReleased();

	void SetFigure(FName CharacterID);
	void SetSlotIndex(int32 InSlotIndex);

	void OpenInfoWidget();
	void CloseInfoWidget();

private:
	void OnFigureClicked();
	void OnFigureLongPressed();
	void OnFigureLongReleased();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Figure;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Figure;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRLobbyFigureInfoWidget> FigureInfoWidget;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> Anim_Hover;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> Anim_HoverMain;

	UPROPERTY(EditAnywhere, Category = "LR|UI")
	TObjectPtr<class UTexture2D> EmptySlotTexture;

private:
	FTimerHandle LongPressTimerHandle;
	FName CurrentCharacterID = NAME_None;
	int32 SlotIndex = -1;
	bool bIsLongPressTriggered = false;

	FVector2D InfoPosition;
};
