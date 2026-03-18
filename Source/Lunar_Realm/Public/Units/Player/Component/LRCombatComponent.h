// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LRCombatComponent.generated.h"

//=============================================================================
// (260205) BJM 제작. 전투 관련 로직 생성(수동, 자동).
// (260206) BJM 자동 전투 상태 enum 추가.
// (260209) BJM 자동전투 최적화 적용 (Tick 로직 분리 및 캐싱)
// (260210) BJM 전투 로직 수정 (타겟 탐색 및 이동, 공격 로직 분리)
// (260224) BJM 적 처치시 중앙에서 비빔 / 자동모드시 코어 타겟 버그 수정
// (260224) BJM 헬퍼 함수 분리 및 구조 정리
//=============================================================================

class ALRCharacter;

UENUM(BlueprintType)
enum class EAutoCombatState : uint8
{
	Manual,
	Auto,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNAR_REALM_API ULRCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULRCombatComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ============================================================================
	// 퍼블릭 인터페이스 (Public Interface)
	// ============================================================================
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAutoMode(bool bInEnableAuto);

	UFUNCTION(BlueprintCallable)
	void ToggleAutoMode() { SetAutoMode(!IsAutoMode()); }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateWeaponInfo(FName InWeaponID);

	UFUNCTION(BlueprintPure, Category = "Combat")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsAutoMode() const { return CombatState == EAutoCombatState::Auto; }

	// ============================================================================
	// 전투 메인 로직 (Combat Main Logic)
	// ============================================================================
protected:
	void OnCombatLogicTimer();
	void ProcessCombatLogic(ALRCharacter* InOwnerCharacter, float InDeltaTime);
	void AttemptAction(float InDeltaTime);
	void MoveToTarget(float InDeltaTime);
	void FindBestTarget();

	// ============================================================================
	// 헬퍼 함수 (Helper Functions)
	// ============================================================================
private:
	void UpdateTargetIndicator(ALRCharacter* InOwnerCharacter);
	void CheckAndClearDeadTarget();
	bool IsTargetDead(AActor* InTargetActor) const;

	ALRCharacter* GetOwnerCharacter() const;
	bool IsTargetInRange() const;
	FGameplayTag GetEnemyRootTag() const;

public:
	void ClearTarget();

protected:
	bool TryExcuteSkill(ALRCharacter* InOwnerCharacter);

protected:
	bool TryAutoSummon(ALRCharacter* InOwnerCharacter);

public:
	// 캐릭터 DT에서 AttackType을 읽어와 사거리를 세팅하는 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void UpdateAttackRange();
protected:
	// 데칼 초기화 및 크기 업데이트 헬퍼 함수
	/*void UpdateRangeDecalSize();*/

	// 메쉬 크기 갱신 헬퍼 함수
	void UpdateRangeMeshSize(float InRange);
	void CreateRangeIndicator();



	// ============================================================================
	// 멤버 변수 (Properties)
	// ============================================================================
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	EAutoCombatState CombatState = EAutoCombatState::Manual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CachedEnemyBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SearchRadius = 1000.0f;

	float CurrentAttackCooldown = 1.0f;
	FTimerHandle CombatLogicTimerHandle;

private:
	float AutoSkillDelay = 0.0f;

protected:
	// 사거리 표시용 스태틱 메쉬 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Indicator")
	class UStaticMeshComponent* RangeMesh;

	// 블루프린트에서 머티리얼을 꽂아줄 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Indicator")
	class UMaterialInterface* RangeMaterial;

	UPROPERTY()
	TObjectPtr<class ALREnemyCharacter> PreviousEnemyTarget = nullptr;

protected:
	bool CheckAndUseAutoHeal(class ALRPlayerCharacter* InPlayerChar);



};
