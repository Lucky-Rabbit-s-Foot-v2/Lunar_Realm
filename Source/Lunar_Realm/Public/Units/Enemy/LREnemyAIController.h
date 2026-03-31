// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRAIController.h"
#include "LREnemyAIController.generated.h"


/**
 * LRAIController 구성 요소
 * Enemy 기본 컨트롤러 (보스는 따로 구현 예정)
 * 
 * [행동 우선순위]
 * 1. 탐지 범위 내에 Player.Character.Player 또는 Player.Character.Member 태그를 
 *    가진 캐릭터가 있으면 → 해당 캐릭터에게 이동 & 공격
 * 2. 없으면 → Player.Structure.Core 태그를 가진 액터로 이동 & 공격
 *
 * [태그 비교]
 * - 모든 태그 비교는 LRTags 네임스페이스(LRGameplayTags.h)를 사용.
 * - 하드코딩된 FName("...") 문자열을 금지.
 * 
 * [향후 계획]
 * - RVO(DetourCrowd)는 부모 클래스 ALRAIController(ADetourCrowdAIController)에서 지원
 */
 //============================================================================
 // (260204) KWB 제작.
 // (260211) KWB 생성자에서 적대 태그 세팅.
 // (260212) KWB BT 하드 코딩 제거, EnemyCharacter가 StaticData에서 로드 & 설정으로 로직 수정
 // (260303) KWB 부모 클래스 TryAttackTarget() 함수 Override 구현 추가
 // (260312) KWB BT 설정을 위한 데이터 초기화 함수 추가
 // (260313) KWB 캐릭터, 멤버 탐지 범위를 공격 범위와 연동 및 안전용 오프셋 멤버 추가, TryAttackTarget 반환 타입 bool -> FGameplayTag 변경
 // (260324) KWB 개별 공격 몽타주, 태그 사용에서 배열로 로직 변경, 보스 전용 페이즈 기반 공격 시도 함수 추가
 //============================================================================

UCLASS()
class LUNAR_REALM_API ALREnemyAIController : public ALRAIController
{
	GENERATED_BODY()
	
public:
	ALREnemyAIController();

	virtual FGameplayTag  TryAttackTarget(AActor* Target) override;

	FGameplayTag TryAttackTargetByPhase(AActor* Target, int32 Phase);

	void InitializeFromEnemyData(FName EnemyID);

	virtual void OnPoolDeactivate_Implementation() override;

private:
	bool IsSkillOnCooldown(UAbilitySystemComponent* ASC, FGameplayTag SkillTag) const;

private:
	static constexpr float DetectionRadiusOffset = 500.0f;

	UPROPERTY()
	TArray<TObjectPtr<UAnimMontage>> CachedAttackMontages;
	TArray<FGameplayTag> CachedSkillTags;

	bool bHasSpecialSkill = false;
};
