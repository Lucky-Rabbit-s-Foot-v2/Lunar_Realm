// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "Data/LRDataStructs.h"
#include "LRPlayerState.generated.h"


/**
 * 
 */

//=============================================================================
// (260203) BJM 제작. 플레이어 스테이트.
// (260205_BJM) DataSubsystem연동 및 장비 장착/해제 기능 추가.
// (260206_BJM) 수정된 ID 테이블 반영.
//=============================================================================

class UAbilitySystemComponent;
class UAttributeSet;
class UGameDataSubsystem;


UCLASS()
class LUNAR_REALM_API ALRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ALRPlayerState();

protected:
	virtual void BeginPlay() override;

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// 데이터 초기화 및 로드
	void InitializePlayerData();

	void EquipItem(EEquipmentSlotType Slot, int32 ItemID);
	void UnequipItem(EEquipmentSlotType Slot);

// 내부 로직
protected:
	void InitializeAttributes();		// 스탯 계산
	void GrantCharacterAbilities();		// 캐릭터 스킬 부여
	void GrantEquipmentAbilities(EEquipmentSlotType Slot, int32 EquipmentID);	// 장비 스킬 부여

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<ULRPlayerAttributeSet> AttributeSet;

protected:
	int32 CharacterID = 10101;
	int32 CharacterLevel = 1;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	TMap<EEquipmentSlotType, int32> EquippedItems;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	TMap<EEquipmentSlotType, int32> EquippedItemLevels;

	// 부여된 스킬 핸들 관리
	TMap<EEquipmentSlotType, TArray<FGameplayAbilitySpecHandle>> EquipmentAbilityHandles;
	TArray<FGameplayAbilitySpecHandle> CharacterAbilityHandles;
};
