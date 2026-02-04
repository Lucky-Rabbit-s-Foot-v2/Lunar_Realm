// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "Component/LRSummonComponent.h"
#include "GameplayTagContainer.h"
#include "LRPlayerCharacter.generated.h"


class ULRInputConfig;
/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ALRPlayerCharacter : public ALRCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ALRPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComponent;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<ULRInputConfig> InputConfig;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//class UInputAction* SummonAction_1;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//class UInputAction* SummonAction_2;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//class UInputAction* SummonAction_3;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	//class UInputAction* SummonAction_4;


	void Move(const FInputActionValue& Value);
	void Input_Summon(FGameplayTag InputTag);
	//void SummonSlot1();
	//void SummonSlot2();
	//void SummonSlot3();
	//void SummonSlot4();

public:
	UPROPERTY()
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UAttributeSet* AttributeSet;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<ULRSummonComponent> SummonComponent;

};
