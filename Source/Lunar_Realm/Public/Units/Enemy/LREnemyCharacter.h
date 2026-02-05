// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemInterface.h"
#include "LREnemyCharacter.generated.h"

class ULREnemyAttributeSet;

/**
 * 적 캐릭터(Enemy) 베이스 클래스
 * - Attributes 초기화
 */
 //============================================================================
 // (260204) KWB 제작. 제반 사항 구현.
 // (260205) DataSubsystem 구조체에서 Attribute 별 값 받아와 초기화하는 로직 구현(ING)
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALREnemyCharacter : public ALRCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ALREnemyCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {
		return AbilitySystemComponent;
	}

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitializeAttributes();

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|ASC")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Attribute")
	TObjectPtr<ULREnemyAttributeSet> AttributeSet = nullptr;
};
