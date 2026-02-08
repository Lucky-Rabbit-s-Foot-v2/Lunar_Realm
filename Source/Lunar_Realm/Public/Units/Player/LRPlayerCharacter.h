// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "Units/Player/Component/LRCombatComponent.h"

#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "Component/LRSummonComponent.h"
#include "Components/DecalComponent.h"
#include "GameplayTagContainer.h"

#include "LRPlayerCharacter.generated.h"

//=============================================================================
// (260203) BJM 제작. 플레이어 캐릭터.
// (260205_BJM) 전투 컴포넌트 추가.
// (260208_BJM) 타겟 락온 기능 추기
//=============================================================================

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UDecalComponent> TargetIndicator;


	void Move(const FInputActionValue& Value);
	void Input_Summon(FGameplayTag InputTag);


public:
	UPROPERTY()
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UAttributeSet* AttributeSet;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<ULRSummonComponent> SummonComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<ULRCombatComponent> CombatComponent;

public:
	// 블루프린트에서 테스트용 스킬 부여 함수
	UFUNCTION(BlueprintCallable, Category = "GAS|Test")
	void GrantTestAbility(TSubclassOf<class UGameplayAbility> AbilityClass);

};
