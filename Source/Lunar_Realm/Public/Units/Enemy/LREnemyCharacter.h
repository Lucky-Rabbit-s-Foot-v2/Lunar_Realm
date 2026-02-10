// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "LREnemyCharacter.generated.h"

class ULREnemyAttributeSet;
class UGameplayAbility;

/**
 * 적 캐릭터(Enemy) 베이스 클래스
 * - Attributes 초기화
 * - 비주얼 데이터 적용 (SkeletalMesh, AnimBP, Scale)
 * - 오브젝트 풀링 라이프사이클 구현
 */
 //============================================================================
 // (260204) KWB 제작. 제반 사항 구현.
 // (260205) KWB DataSubsystem 구조체에서 Attribute 별 값 받아와 초기화하는 로직 구현
 // (260209) KWB "GrantEnemyAbilities()" 함수 리팩터 -> "FEnemyStaticData" 스킬 항목 "TArray<int32>" 타입으로 캐릭터, 장비와 통일
 // (260210) KWB 키값 타입 int32 -> FName 으로 변경 반영
 // (260210) KWB 비주얼 데이터 적용, 풀링 라이프 사이클 구현
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

	UFUNCTION(BlueprintCallable)
	void OnDie();

	UFUNCTION(BlueprintCallable)
	void InitializeByEnemyID(FName EnemyID);

	void GrantEnemyAbilities();

	void ClearGrantedEnemyAbilities();

	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitializeAttributes(FName EnemyID);

	void ApplyVisualData(const struct FEnemyStaticData& EnemyData);

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|ASC")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Attribute")
	TObjectPtr<ULREnemyAttributeSet> AttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Enemy", meta = (AllowPrivateAccess = "true"))
	FName CurrentEnemyID;

	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};
