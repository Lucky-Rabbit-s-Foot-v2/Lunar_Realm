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
// (260223) BJM 수정, CachedTarget 타입 LRCharacter -> AActor로 변경\
// (260310) BJM 수정, 공용 태그 삭제 후 CooldownTagContainer로 대체
//=============================================================================

class ALRCharacter;
class ALRProjectile;
struct FSkillObjectInitData;

UCLASS()
class LUNAR_REALM_API ULRGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	ULRGameplayAbilityBase();

protected:
	/*
	 * final로 막아놓고 캐싱 처리
	 */
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override final;
	
	
	/*
	 * 자식 GA들이 오버라이딩할 함수
	 */
	virtual void OnAbilityActivated(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo);
	
	/*
	* SkillID 기준으로 CoolTimeGE를 로드해 SelfASC에 적용
	*  Cooldown 수치는 FSkillEffectData.Cooldown에서 SetByCaller로 주입
	 */
	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// GetCooldownTags 오버라이드
	virtual const FGameplayTagContainer* GetCooldownTags() const override { return &CooldownTagContainer; }
	
	/**
	 * SpawnData 기반으로 투사체를 스폰하는 공통 헬퍼
	 * @param ProjectileClass  스폰할 투사체 클래스
	 * @param InitData         투사체 초기화 데이터
	 */
	void SpawnProjectiles(
		TSubclassOf<ALRProjectile> ProjectileClass,
		const FSkillObjectInitData& InitData);
	
	//테스팅 오버로딩
	void SpawnProjectiles(TSubclassOf<ALRProjectile> ProjectileClass, const FSkillObjectInitData& InitData, FRotator BaseRotation);
	
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
	
	/**
	 * Instigator의 팀 태그를 기반으로 적대 팀 태그 반환
	 * @return 적대 팀의 루트 태그
	 */
	FGameplayTag GetHostileTeamTag() const;
	
	/**
	 * 범위 내 가장 가까운 적대 액터 탐색
	 * @param HostileTag   적대 팀 루트 태그
	 * @param SearchRadius 탐색 반경 (cm)
	 * @return 가장 가까운 적대 액터, 없으면 nullptr
	 */
	AActor* FindNearestHostile(FGameplayTag HostileTag, float SearchRadius) const;
	
	
protected:
	UPROPERTY()
	TObjectPtr<const ALRCharacter> CachedInstigator;
	UPROPERTY()
	TObjectPtr<const AActor> CachedTarget;
	
	FGameplayTagContainer CooldownTagContainer;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillID = NAME_None;
	UPROPERTY(EditDefaultsOnly, Category = "LR|Skill")
	FName SkillEffectID = NAME_None;

};
