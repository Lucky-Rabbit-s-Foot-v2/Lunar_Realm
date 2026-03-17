// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRCharacterCollection.generated.h"


//============================================================================
/**
 * 캐릭터 도감 위젯
 */
 //============================================================================
 // (260303) PJB 제작.
 //============================================================================

UCLASS()
class LUNAR_REALM_API ULRCharacterCollection : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	void AddItemToTileView(class UGameDataSubsystem* GameDataSubsystem, const FName& LockedCharactersID, bool bIsLocked);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTileView> CharacterTileView;

};
