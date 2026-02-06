// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemInterface.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "LRMemberCharacter.generated.h"


class UAbilitySystemComponent;
class ULRPlayerAttributeSet;
/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRMemberCharacter : public ALRCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ALRMemberCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	ULRPlayerAttributeSet* GetAttributeSet() const { return MemberAttributeSet; }

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void OnHealthChanged(float NewValue);

protected:
	virtual void BeginPlay() override;

	virtual void OnHealthChangedNative(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<ULRPlayerAttributeSet> MemberAttributeSet;

};
