// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFigureHovered, FName, CharacterID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFigureUnhovered);

UCLASS()
class LUNAR_REALM_API ULRLobbyFigureWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void OpenUI() override;
	virtual void RefreshUI() override;

	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureClicked OnFigureClickedDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureHovered OnFigureHoveredDel;
	UPROPERTY(BlueprintAssignable, Category = "LR|UI|Events")
	FOnFigureUnhovered OnFigureUnhoveredDel;

	UFUNCTION()
	void OnFigureButtonClicked();
	UFUNCTION()
	void OnFigureButtonHovered();
	UFUNCTION()
	void OnFigureButtonUnhovered();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Figure;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Figure;

	UPROPERTY(EditAnywhere, Category = "LR|UI")
	TSubclassOf<class ULRLobbyFigureInfoWidget> FigureInfoWidgetClass;

private:
	FName CurrentCharacterID;
};
