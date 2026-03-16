// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Animation/AnimMontage.h"
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
 // (260303) KWB DropAether 값 DT에서 조회하는 헬퍼 함수 추가, "OnDie()"시 게임 스테이트에 에테르 추가 로직 추가
 // (260303) KWB bool IsDead 변수로 OnDie() 중복 호출 방어 
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

	void PlayAttackedMontage();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void InitializeAttributes(FName EnemyID);

	void ApplyVisualData(const struct FEnemyStaticData& EnemyData);

	void PlayDeathMontage();

	UFUNCTION()
	void OnAttackedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 몽타주 완료 후 호출되는 후속 처리 함수
	void FinishDeathSequence();

protected:
	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedAttackedMontage;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CachedDeathMontage;

private:
	UPROPERTY(EditDefaultsOnly, Category = "LR|ASC")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "LR|Attribute")
	TObjectPtr<ULREnemyAttributeSet> AttributeSet = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LR|Enemy", meta = (AllowPrivateAccess = "true"))
	FName CurrentEnemyID;

	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	float GetDropAetherAmount() const;

	bool IsDead = false;

	// (260316) BJM: 타겟팅 마커(머리 위 화살표) UI 연동을 위한 함수 및 변수 추가
public:
	// 외부(Player)에서 타겟 마커를 켜고 끌 때 부르는 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetTargetMarkerVisibility(bool bVisible);

	// 생성자에서 타겟 마커 설정값 가져오는 핼퍼함수
	void SetupTargetMarker();

protected:
	// 머리 위에 띄울 2D 화살표 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UStaticMeshComponent* TargetMarkerMesh;
};
