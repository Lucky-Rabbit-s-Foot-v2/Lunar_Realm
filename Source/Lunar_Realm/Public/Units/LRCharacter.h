// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/LRPoolableInterface.h"
#include "GameplayTagContainer.h"
#include "LRCharacter.generated.h"

/**
 * LRCharacter 구성 요소
 * 모든 캐릭터의 베이스 클래스
 * - 세이브 데이터 관리
 * - 오브젝트 풀링 처리
 */
//============================================================================
// (260127) PJB 제작. 제반 사항 구현.
// (260205_BJM) GetUnitTag() 추가.
//============================================================================

UCLASS()
class LUNAR_REALM_API ALRCharacter : public ACharacter, public ILRPoolableInterface
{
	GENERATED_BODY()

public:
	ALRCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeByData() {};
	virtual void SaveData() {};
	virtual void LoadData() {};

	virtual void OnPoolActivate_Implementation();
	virtual void OnPoolDeactivate_Implementation();

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	* 오브젝트 풀링 헬퍼 함수
	* - 자손 클래스의 베이스 클래스 타입 반환
	*/
	UFUNCTION(BlueprintPure)
	TSubclassOf<ALRCharacter> GetBaseClass() const { return TSubclassOf<ALRCharacter>(GetClass()); }

public:
	UFUNCTION(BlueprintCallable, Category = "Tags")
	FGameplayTag GetUnitTag() const { return UnitTag; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTag UnitTag;
};
