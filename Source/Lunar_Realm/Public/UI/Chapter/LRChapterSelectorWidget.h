// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRChapterSelectorWidget.generated.h"


//============================================================================
/**
 * 챕터 선택 UI 위젯
 * - 여러 챕터 표시하고 선택 가능
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRChapterSelectorWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OnBackButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRChapterWidget> Chapter1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRChapterWidget> Chapter2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRChapterWidget> Chapter3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Back;

	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class ULRLobbyWidget> LobbyWidgetClass;
};
