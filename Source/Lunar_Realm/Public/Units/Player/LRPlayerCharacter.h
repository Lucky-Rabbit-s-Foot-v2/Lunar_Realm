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
// (260205) BJM 전투 컴포넌트 추가.
// (260208) BJM 타겟 락온 기능 추기
// (260210) BJM 카메라 컴포넌트 추가로 인한 스프링암, 카메라 컴포넌트 주석처리
// (260212) BJM 사망 처리 추가
// (260223) BJM 사망 후 조작 적용
// (260227) BJM beginplay에서 무기 장착
//=============================================================================

class ULRInputConfig;
class ALREquipmentBase;
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

	void ToggleAutoMode();
	void UsePotion();

	/** 소환 슬롯 테스트를 위한 함수 : 정적 데이터 입력 */
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Test")
	void TestSummonSlot();

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

protected:

	void Move(const FInputActionValue& Value);
	void Input_Summon(FGameplayTag InputTag);
	void Input_Charge(const FInputActionValue& Value);
	virtual void OnHealthChangedNative(const FOnAttributeChangeData& Data);

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

public:
	virtual void Die();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	bool bIsDead = false;

	UFUNCTION()
	void RespawnPlayer();

	FTimerHandle RespawnTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Respawn")
	float RespawnTime = 5.0f;

public:
	float GetCameraOffsetY() const { return CameraOffsetY; }

protected:
	float CameraOffsetY = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Setting")
	float DeadCameraSpeed = 1000.0f;

public:
public:
	bool IsInvincible() const { return bIsInvincible; }

protected:
	bool bIsInvincible = false;

	FTimerHandle BlinkTimerHandle;
	FTimerHandle InvincibilityTimerHandle;

	UFUNCTION()
	void  OnBlinkTimer();

	UFUNCTION()
	void EndInvincibility();

public:
	// PlayerState가 무기를 바꿀 때마다 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "LR|Equipment")
	void UpdateWeaponMesh(FName InWeaponID);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "LR|Equipment")
	TSubclassOf<ALREquipmentBase> WeaponClass;

	UPROPERTY()
	TObjectPtr<ALREquipmentBase> CurrentWeaponActor;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Equipment")
	FName WeaponSocketName = TEXT("WeaponSocket");
};
