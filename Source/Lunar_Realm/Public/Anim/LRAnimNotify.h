// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "LRAnimNotify.generated.h"

/**
 * 
 */

//=============================================================================
// (260224) BJM 제작 애님 노티파이 추가
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	ULRAnimNotify();
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Notify")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Notify")
	float VolumeMultiplier = 1.0f;
};
