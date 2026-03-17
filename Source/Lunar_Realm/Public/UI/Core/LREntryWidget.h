// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "LREntryWidget.generated.h"


//============================================================================
/**
 * 도감용 엔트리 위젯 클래스
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS(BlueprintType)
class LUNAR_REALM_API ULRTileData : public UObject
{
	GENERATED_BODY()

public:
	void SetID(const FName& InID) { ID = InID; }
	void SetIcon(UTexture2D* InIcon) { Icon = InIcon; }
	void SetFrame(UTexture2D* InFrame) { Frame = InFrame; }
	void SetIsLocked(bool bInIsLocked) { bIsLocked = bInIsLocked; }

	UFUNCTION(BlueprintCallable, Category = "LR|Tile Data")
	FName GetID() const { return ID; }

	UFUNCTION(BlueprintCallable, Category = "LR|Tile Data")
	UTexture2D* GetIcon() const { return Icon; }

	UFUNCTION(BlueprintCallable, Category = "LR|Tile Data")
	UTexture2D* GetFrame() const { return Frame; }

	bool IsLocked() { return bIsLocked; }

private:
	UPROPERTY()
	FName ID;

	UPROPERTY()
	TObjectPtr<class UTexture2D> Icon;

	UPROPERTY()
	TObjectPtr<class UTexture2D> Frame;

	UPROPERTY()
	bool bIsLocked = true;
};

UCLASS()
class LUNAR_REALM_API ULREntryWidget : public ULRBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void RefreshData();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Frame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Locked;

	UPROPERTY()
	TObjectPtr<ULRTileData> TileData;
};
