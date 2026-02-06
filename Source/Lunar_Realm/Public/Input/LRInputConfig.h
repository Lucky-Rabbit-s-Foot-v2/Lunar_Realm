// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "LRInputConfig.generated.h"

//=============================================================================
// (260206) BJM 제작. Player Input 관련 Component로 관리하기 위한 Config.
// =============================================================================

USTRUCT(BlueprintType)
struct FLRInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};



/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FLRInputAction> AbilityInputActions;
	
};
