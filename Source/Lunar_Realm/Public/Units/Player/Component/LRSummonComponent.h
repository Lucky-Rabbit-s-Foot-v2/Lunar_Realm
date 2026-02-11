// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LRSummonComponent.generated.h"

class ALRPlayerCore;
class ALRMemberCharacter;

//=============================================================================
// (260204) BJM 제작. 소환컴포넌트.
// (260211) BJM 오브젝트 풀링시스템 적용
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

	UFUNCTION(BlueprintCallable, Category = "Summon")
	void TrySummonUnit(int32 InSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Summon") 
	void LoadDeckData(const TArray<FName>& InUnitIDs);


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TArray<FName> SummonDeck;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	TSubclassOf<ALRMemberCharacter> BaseMemberClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Summon")
	TObjectPtr<ALRPlayerCore> TargetCore;

	UPROPERTY(EditAnywhere, Category = "Summon")
	float SpawnDistance = 200.0f;

private:
	bool IsSummonValid(int32 InSlotIndex) const;
	void ProcessSummon(FName InTargetUnitID);


};
