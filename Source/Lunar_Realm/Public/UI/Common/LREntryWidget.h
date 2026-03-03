// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRChildWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "LREntryWidget.generated.h"


//============================================================================
/**
 * 도감용 엔트리 위젯 클래스
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULREntryWidget : public ULRChildWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void SetData(const FString& InID, class UImage* InIcon);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon;

	UPROPERTY(VisibleAnywhere, Category = "LR|EntryWidget")
	FString ID;
};
