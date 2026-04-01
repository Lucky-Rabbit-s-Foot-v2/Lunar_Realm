// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "Data/LREnumType.h"
#include "Data/LRDataStructs.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "LREntryWidget.generated.h"


//============================================================================
/**
 * 도감용 엔트리 위젯 클래스
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileClicked, ULREntryWidget*, InWidget);

UCLASS(BlueprintType)
class LUNAR_REALM_API ULRTileData : public UObject
{
	GENERATED_BODY()

public:
	ULRTileData() = default;
	ULRTileData(const FName& InID, ECollectionType InType, UTexture2D* InIcon, UTexture2D* InFrame, bool bInIsLocked = true)
		: ID(InID), Type(InType), Icon(InIcon), Frame(InFrame), bIsLocked(bInIsLocked) 
	{
	}
	
	UPROPERTY()
	FName ID = NAME_None;

	UPROPERTY()
	ECollectionType Type = ECollectionType::NONE;

	UPROPERTY()
	TObjectPtr<class UTexture2D> Icon = nullptr;

	UPROPERTY()
	TObjectPtr<class UTexture2D> Frame = nullptr;

	UPROPERTY()
	bool bIsLocked = true;

	UPROPERTY()
	bool bIsSelected = false;
};

UCLASS()
class LUNAR_REALM_API ULREntryWidget : public ULRBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeConstruct() override;
	virtual void BindProperties() override;
	virtual void UnbindProperties() override;

	virtual void RefreshData();

	ULRTileData* GetTileData() const { return TileData; }

	virtual void SetSelected(bool bSelected);

	virtual void SetType(ECollectionType InType);

	UPROPERTY(BlueprintAssignable)
	FOnTileClicked OnTileClickedDel;

	UFUNCTION()
	virtual void OnTileClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Frame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Locked;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Black;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Selected;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Selected;

	UPROPERTY()
	TObjectPtr<ULRTileData> TileData;
};
