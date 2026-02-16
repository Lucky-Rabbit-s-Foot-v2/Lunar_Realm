// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/LRDataStructs.h"
#include "LRSummonComponent.generated.h"


class ALRPlayerCore;
class ALRMemberCharacter;
class ULRPlayerAttributeSet;
class ALRPlayerState;

// 몇번 슬롯이 소환됐고, 쿨타임은 몇초인지
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitSummonedDelegate, int32, SlotIndex, float, CooldownTime);

//=============================================================================
// (260204) BJM 제작. 소환컴포넌트.
// (260211) BJM 오브젝트 풀링시스템 적용
// (260213) BJM 비용/쿨타임/회전 로직 추가 및 헬퍼 함수 분리
// (260216) BJM SummonComponent 쿨타임 ui 적용
//=============================================================================



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUNAR_REALM_API ULRSummonComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULRSummonComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 소환 시도
	UFUNCTION(BlueprintCallable, Category = "Summon")
	void TrySummonUnit(int32 InSlotIndex);

	// 덱 데이터 로드
	UFUNCTION(BlueprintCallable, Category = "Summon") 
	void LoadDeckData(const TArray<FName>& InUnitIDs);

	// 남은 쿨타임 반환 (UI 갱신용)
	UFUNCTION(BlueprintCallable, Category = "Summon")
	float GetRemainingCooldown(FName InUnitID) const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Summon|Event")
	FOnUnitSummonedDelegate OnUnitSummoned;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TArray<FName> SummonDeck;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	TSubclassOf<ALRMemberCharacter> BaseMemberClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<ALRPlayerCore> TargetCore;

	// 각 유닛 ID별 마지막 소환 시간 기록 (쿨타임 계산용)
	UPROPERTY()
	TMap<FName, double> LastSummonTimeMap;

	UPROPERTY(EditAnywhere, Category = "Summon")
	float SpawnDistance = 200.0f;

private:
	void ExecuteSummon(const FCharacterStaticData& InCharData, int32 InSlotIndex);
	void ProcessSummon(const FCharacterStaticData& InCharData);

private:
	void FindPlayerCore();

	const FCharacterStaticData* GetCharacterData(FName InUnitID) const;
	ALRPlayerState* GetPlayerState() const;
	ULRPlayerAttributeSet* GetAttributeSet() const;

	bool IsValidSummonRequest(int32 InSlotIndex, FName& OutUnitID, const FCharacterStaticData*& OutCharData);
	bool IsOnCooldown(FName InUnitID, float InCoolDownTime) const;
	bool CanAffordSummon(float InCost) const;

	void DeductSummonCost(float InCost);
	void UpdateLastSummonTime(FName InUnitID);
	void NotifySummonSuccess(int32 InSlotIndex, float InCooldownTime); 
	FTransform CalculateSpawnTransform() const;

public:
	//UPROPERTY(BlueprintAssignable, Category = "Summon|Event")
	//FOnUnitSummonedDelegate OnUnitSummoned;

	//UFUNCTION(BlueprintCallable, Category = "Summon")
	//float GetRemainingCooldown(FName InUnitID) const;

protected:


};
