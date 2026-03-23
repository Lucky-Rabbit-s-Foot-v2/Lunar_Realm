// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "LRMMC_Damage.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRMMC_Damage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	ULRMMC_Damage();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;


};
