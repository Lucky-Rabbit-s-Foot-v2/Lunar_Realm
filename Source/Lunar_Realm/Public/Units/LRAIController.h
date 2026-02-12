// Fill out your copyright notice in the Description page of Project Settings.

/* TEMP */

#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "GameplayTagContainer.h"
#include "LRAIController.generated.h"

class ALRCore;
class ALRCharacter;
class UBehaviorTree;
class UBlackboardComponent;

/**
 * Blackboard 키 이름 상수
 *
 * BT 노드(Service, Task, Decorator)와 AIController 참조 용도
 * 문자열 사용 대신 상수 사용 목적
 */
namespace LRBBKeys
{
	inline const FName TargetActor = TEXT("TargetActor");			 // Object (AActor* - 베이스 클래스)
	inline const FName TargetCore = TEXT("TargetCore");				 // Object (AActor*)
	inline const FName HasNearbyHostile = TEXT("HasNearbyHostile");  // Bool
}

/**
 * AIController 구성 요소
 * - 모든 자동 전투 유닛(Enemy, Member)의 공통 AI 컨트롤러
 * - BT_AutoCombat과 연동
 * 
 * [자식 클래스가 해야 할 일]
 * 코드 :
 *  생성자에서 세팅
 *   - HostileRootTag : 적대 진영 루트 태그 (예: "Player" 또는 "Enemy")
 *   - TargetCoreTag  : 폴백 공격 대상 코어 태그 (예: "Player.Structure.Core")
 * 엔진 :
 *- BehaviorTreeAsset 세팅
 * 
 * [행동 우선순위]
 * 1. DetectionRadius 내 HostileRootTag 계열 캐릭터 → 가장 가까운 것 추적 & 공격
 * 2. 없으면 → TargetCoreTag를 가진 코어로 이동 & 공격
 */
 //============================================================================
 // (260204) KWB 제작.
 // (260211) KWB 제반 사항 구현. (공통 AI 로직 통합. Enemy/Member 공용)
 //============================================================================
UCLASS()	
class LUNAR_REALM_API ALRAIController : public ADetourCrowdAIController
{
	GENERATED_BODY()
	
public:
	ALRAIController();

	// === BT Node API (BTService, BTTask에서 호출) ===
	UFUNCTION(BlueprintCallable, Category = "LR|AI")
	AActor* FindNearestHostile() const;

	UFUNCTION(BlueprintCallable, Category = "LR|AI")
	AActor* FindTargetCore() const;

	UFUNCTION(BlueprintCallable, Category = "LR|AI")
	bool TryAttackTarget(AActor* Target);

	UFUNCTION(BlueprintPure, Category = "LR|AI")
	float GetAttackRange() const { return AttackRange; }

	UFUNCTION(BlueprintPure, Category = "LR|AI")
	float GetDetectionRadius() const { return DetectionRadius; }

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual FGameplayTag GetHostileRootTag() const;
	virtual FGameplayTag GetTargetCoreTag() const;

	AActor* FindActorWithGameplayTag(TSubclassOf<AActor> ActorClass, const FGameplayTag& Tag) const;

	// === 설정값 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Detection")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Combat")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Combat")
	float AttackCooldown = 1.5f;

	// === 태그 (자식 클래스가 생성자에서 설정) ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Tags")
	FGameplayTag HostileRootTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Tags")
	FGameplayTag TargetCoreTag;

private:
	float LastAttackTime = 0.0f;
};
