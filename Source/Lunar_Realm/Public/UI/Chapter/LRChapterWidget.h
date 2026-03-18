// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRChapterWidget.generated.h"


//============================================================================
/**
 * 챕터 UI 위젯
 * - 챕터 정보 들고 있어야 함.
 */
 //============================================================================
 // (260213) PJB 제작. 제반 사항 구현
 //============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChapterOpenClicked);

UCLASS()
class LUNAR_REALM_API ULRChapterWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;
	
	virtual void RefreshUI() override;

	void SetChapterID(FName InChapterID);

	UPROPERTY(BlueprintAssignable, Category = "LR|Event")
	FOnChapterOpenClicked OnChapterOpenClickedDel;

private:
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void OnOpenButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Open;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Chapter")
	FName ChapterID;

	UPROPERTY(EditAnywhere, Category = "LR|UI")
	TSubclassOf<class ULRStagePageWidget> StageSelectorWidgetClass;
};
