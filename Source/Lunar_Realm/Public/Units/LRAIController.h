#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "GameplayTagContainer.h"
#include "Interfaces/LRPoolableInterface.h"
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
	inline const FName TargetActor = TEXT("TargetActor");				// Object (AActor* -> 베이스 클래스)
	inline const FName TargetCore = TEXT("TargetCore");					// Object (AActor*)
	inline const FName HasNearbyHostile = TEXT("HasNearbyHostile");		// Bool
	inline const FName CoreInRange = TEXT("CoreInRange");				// Bool
	inline const FName CurrentPhase = TEXT("CurrentPhase");				// Int
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
 // (260212) KWB 비헤이비어트리 설정을 데이터 드리븐 구조로 전환하기 위한 SetAndRunBehaviorTree 함수 추가. OnPossess()에서 BT 검사 및 실행 로직 제거.
 // (260224) KWB 컨트롤러 풀링 시스템 적용을 위한 인터페이스 구현 추가
 // (260225) KWB 주석 처리된 미사용 코드 제거
 // (260303) KWB Enemy, Member 각기 다른 스킬 사용을 위한 TryAttackTarget() 함수 가상 함수화
 // (260311) KWB 크라우드 우회 매니저 사용한 군중 회피 시스템 적용
 // (260312) KWB 에너미 타입 세분화를 위한 BBKey, 보스 현재 페이즈 구분을 위한 BBKey 추가
 // (260313) KWB TryAttackTarget 반환 타입 bool -> FGameplayTag 변경 (BT Task InProgress 연동)
 //============================================================================
UCLASS()	
class LUNAR_REALM_API ALRAIController : public ADetourCrowdAIController, public ILRPoolableInterface
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
	virtual FGameplayTag TryAttackTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "LR|AI")
	void InitializeBehaviorTree(UBehaviorTree* NewBT);

	UFUNCTION(BlueprintPure, Category = "LR|AI")
	float GetAttackRange() const { return AttackRange; }

	UFUNCTION(BlueprintPure, Category = "LR|AI")
	float GetDetectionRadius() const { return DetectionRadius; }

	void SetDetectionRadius(float InRadius) { DetectionRadius = InRadius; }

	// 인터페이스 구현
	virtual void OnPoolActivate_Implementation() override;
	virtual void OnPoolDeactivate_Implementation() override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual FGameplayTag GetHostileRootTag() const;
	virtual FGameplayTag GetTargetCoreTag() const;

	AActor* FindActorWithGameplayTag(TSubclassOf<AActor> ActorClass, const FGameplayTag& Tag) const;

	// === 설정값 ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|BehaviorTree")
	UBehaviorTree* BehaviorTreeAsset = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Detection")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Combat")
	float AttackRange = 300.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Combat")
	float AttackCooldown = 1.5f;

	// === 태그 (자식 클래스가 생성자에서 설정) ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Tags")
	FGameplayTag HostileRootTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI|Tags")
	FGameplayTag TargetCoreTag;

	float LastAttackTime = 0.0f;
};
