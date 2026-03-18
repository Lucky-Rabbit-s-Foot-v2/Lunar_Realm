// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "LRBTTAttack.generated.h"

// =============================================================================
/**
 * LRBTTAttack 테스크
 *
 * [동작]
 * 1. BB에서 TargetActor를 읽는다
 * 2. 공격 사거리(AttackRange) 안인지 확인한다
 * 3. 사거리 안이면 => AIController -> TryAttackTarget() 호출
 * 4. 타겟을 바라본 뒤 결과를 반환한다
 *
 */
 //=============================================================================
 // (260211) KWB 제작. 제반 사항 구현.
 // (260303) BJM 수정, TryAttackTarget() 호출로 변경
 // (260312) KWB 수정. Enemy 종류 세분화에 따른 엔진에서 타겟 설정용 멤버 추가
 // (260313) KWB 수정. BT 루프 방지를 위한 InProgress 패턴 도입. NodeMemory 구조체 추가. TryAttackTarget 반환 타입 bool -> FGameplayTag 변경.
 // =============================================================================

// 에너미별 Task 상태를 저장하는 구조체
struct FLRBTAttackTaskMemory
{
	FDelegateHandle AbilityEndedHandle;
	FDelegateHandle AbilityFailedHandle;
	FDelegateHandle CooldownTagHandle;
	FGameplayTag CooldownTag;
	TWeakObjectPtr<UBehaviorTreeComponent> BTComp;
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayTag ActivatedAbilityTag;
	bool bAbilityEndedSynchronously = false;
	bool bAbilityFailedSynchronously = false;
};

UCLASS()
class LUNAR_REALM_API ULRBTTAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	ULRBTTAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// BT가 Task를 강제 중단할 때 호출 (델리게이트 정리용)
	virtual EBTNodeResult::Type AbortTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// NodeMemory 크기/초기화/정리
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;

private:
	// GA 종료 시 BT에 완료 신호를 보내는 콜백
	void OnAbilityEnded(const FAbilityEndedData& EndedData, UBehaviorTreeComponent* BTComp, uint8* NodeMemory);
	void OnAbilityFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailureTags, UBehaviorTreeComponent* BTComp, uint8* NodeMemory);
	void OnCooldownTagChanged(const FGameplayTag Tag, int32 Count, UBehaviorTreeComponent* BTComp, uint8* NodeMemory);

	// 델리게이트 등록 해제 공통 처리
	void UnregisterDelegate(FLRBTAttackTaskMemory* Memory);

public:
	// 엔진에서 직접 타겟 설정할 용도의 멤버
	UPROPERTY(EditAnywhere, Category = "LR|Attack")
	FBlackboardKeySelector TargetKey;
};
