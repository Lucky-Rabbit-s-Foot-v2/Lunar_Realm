// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LREquipmentBase.generated.h"

UCLASS()
class LUNAR_REALM_API ALREquipmentBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALREquipmentBase();
	
	bool InitEquipment(FName InEquipmentID);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "LR|Equipment")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "LR|Equipment")
	FName CachedEquipmentID;
	
	
};
