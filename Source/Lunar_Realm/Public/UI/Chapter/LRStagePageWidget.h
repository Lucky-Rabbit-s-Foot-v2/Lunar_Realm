// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRPageWidget.h"
#include "LRStagePageWidget.generated.h"

//============================================================================
/**
 * 스테이지 선택 UI 위젯
 * - 여러 스테이지 표시하고 선택 가능
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRStagePageWidget : public ULRPageWidget
{
	GENERATED_BODY()
	
public:
	virtual void InitializeUI() override;

	virtual void RegisterSubWidgets() override;

	UFUNCTION(BlueprintCallable)
	void SetChapterID(FName InChapterID);

	UFUNCTION(BlueprintCallable)
	void SetStageData(const TArray<FName>& StageIDs);

	UFUNCTION()
	void OnChapterSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_BG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class ULRStageWidget> Stage5;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UComboBoxString> ComboBox;

private:
	FName CurrentChapterID;
};
