// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemInterface.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "Interfaces/LRPoolableInterface.h"
#include "LRMemberCharacter.generated.h"


class UAbilitySystemComponent;
class ULRPlayerAttributeSet;
class ALREquipmentBase;
/**
 * 
 */
//=============================================================================
// (260204) BJM 제작. 소환캐릭터.
// (260211) BJM 오브젝트 풀링시스템 적용, DataStruct 적용
// (260212) BJM 사망, 피아식별, 공격함수, 히트박스, 데미지전달 구현
// (260220) BJM GA 적용
//=============================================================================

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

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

public:
	void InitCharacterData(FName InCharacterID);

private:
	void ResetAttributes();
	void ResetAIController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<ULRPlayerAttributeSet> MemberAttributeSet;



public:
	virtual void Die();

protected:
	// PoolingSystem->ReturnToPool 가져오기 위한 함수
	UFUNCTION()
	void ReturnSelf();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> LoadedDeathMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> LoadedAttackMontage;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Animation")
	UAnimMontage* LoadedHitMontage;


	FTimerHandle DeadTimerHandle;
	bool bIsDead = false;

public:
	void UpdateWeaponMesh(FName InWeaponID);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Weapon")
	TSubclassOf<ALREquipmentBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LR|Weapon")
	FName WeaponSocketName = TEXT("WeaponSocket");

	UPROPERTY()
	ALREquipmentBase* CurrentWeaponActor;
};
