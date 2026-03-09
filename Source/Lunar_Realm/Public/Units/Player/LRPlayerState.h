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
// (260205) BJM DataSubsystem연동 및 장비 장착/해제 기능 추가.
// (260206) BJM 수정된 ID 테이블 반영.
// (260225) BJM player 기본공격 GA 부여 수정 및 스킬 연동
// (260303) KWB GameState로부터 에테르를 받는 함수 추가 및 BeginPlay()에서 델리게이트 바인딩 추가 : 추후 시스템 구성(ex. SpawnManager)에 따라 삭제 예정
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

	void EquipItem(EEquipmentSlotType Slot, FName ItemID);
	void UnequipItem(EEquipmentSlotType Slot);

// 내부 로직
protected:
	void InitializeAttributes();		// 스탯 계산
	void GrantCharacterAbilities();		// 캐릭터 스킬 부여
	void GrantEquipmentAbilities(EEquipmentSlotType Slot, FName EquipmentID);	// 장비 스킬 부여

public:
	void ActivateSkill1();
	void ActivateSkill2();

	// (260303) KWB GameState로부터 에테르를 받는 함수
	void OnAetherReceived(float Amount);

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<ULRPlayerAttributeSet> AttributeSet;

public:
	// 플레이어 데이터가 한 번이라도 초기화되었는지 체크하는 변수
	bool bIsPlayerDataInitialized = false;

	FName GetCharacterID() const { return CharacterID; }

protected:
	FName CharacterID = FName("10101");
	FName CharacterLevel = FName("1");

	UPROPERTY(VisibleAnywhere, Category = "Data")
	TMap<EEquipmentSlotType, FName> EquippedItems;

	UPROPERTY(VisibleAnywhere, Category = "Data")
	TMap<EEquipmentSlotType, int32> EquippedItemLevels;

	// 부여된 스킬 핸들 관리
	TMap<EEquipmentSlotType, TArray<FGameplayAbilitySpecHandle>> EquipmentAbilityHandles;
	TArray<FGameplayAbilitySpecHandle> CharacterAbilityHandles;

public:
	UFUNCTION(BlueprintCallable, Category = "LR | Combat")
	TArray<FName> GetEquippedAutoSkillIDs() const;
};
