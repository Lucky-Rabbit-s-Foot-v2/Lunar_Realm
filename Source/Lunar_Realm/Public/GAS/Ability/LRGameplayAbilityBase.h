// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LRGameplayAbilityBase.generated.h"

/**
 *  LRGameplayAbilityBase
 *  - 프로젝트 내무 모든 GA의 베이스
 * 
 * 주요 기능:
 * - 캐릭터/ASC 접근을 위한 헬퍼
 * - 공통 설정
 */
//=============================================================================
// (260209) BJM 제작. GameplayAbility 기본 클래스 생성.
// (260209) KHS 기본 헬퍼, 공통 설정 추가
//=============================================================================

class ALRCharacter;

UCLASS()
class LUNAR_REALM_API ULRGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	ULRGameplayAbilityBase();
	
protected:
	/*
	 * GA가 보유한 캐릭터 정보 반환
	 * @param : ActorInfo
	 * @return : 캐릭터 포인터, 실패시 nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "LR|Ability")
	ALRCharacter* GetCharacterFromActorInfo(const FGameplayAbilityActorInfo& ActorInfo) const;
	
	/*
	 * GA오너의 ASC 정보 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "LR|Ability")
	UAbilitySystemComponent* GetOwnerASC() const;
	
	
};
