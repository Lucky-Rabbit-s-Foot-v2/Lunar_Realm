// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPopupWidget.h"
#include "LRLobbyFigureWidget.generated.h"


// =============================================================================
/**
 * 로비에 띄울 피규어 위젯
 */
 //=============================================================================
 // (260212) PJB 제작. 
 // (260212) PJB 주의사항 : Unhover 전에 Hover 이벤트가 발생할 수 있음. 
 //=============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFigureClicked, FName, CharacterID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFigureLongPressed, FName, CharacterID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFigureLongReleased);

UCLASS()
class LUNAR_REALM_API ULRLobbyFigureWidget : public ULRPopupWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureClicked OnFigureClickedDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureLongPressed OnFigureLongPressedDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureLongReleased OnFigureLongReleasedDel;

	UFUNCTION()
	void OnFigurePressed();
	UFUNCTION()
	void OnFigureReleased();

private:
	void OnFigureClicked();
	void OnFigureLongPressed();
	void OnFigureLongReleased();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Figure;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Figure;

	UPROPERTY(EditAnywhere, Category = "LR|UI")
	TSubclassOf<class ULRLobbyFigureInfoWidget> FigureInfoWidgetClass;

private:
	FTimerHandle LongPressTimerHandle;
	FName CurrentCharacterID;
	bool bIsLongPressTriggered = false;
};
