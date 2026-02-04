// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Units/Member/LRMemberCharacter.h"
#include "LRSummonComponent.generated.h"


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
	void TrySummonUnit(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Summon") 
	void LoadDeckData(const TArray<int32>&UnitIDs);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Summon")
	TArray<TSubclassOf<ALRMemberCharacter>> SummonDeck;

	UPROPERTY(EditAnywhere, Category = "Summon")
	float SpawnDistance = 200.0f;

};
