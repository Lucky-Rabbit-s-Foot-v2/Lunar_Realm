// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/LRAnimNotify_Footstep.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void ULRAnimNotify_Footstep::Notify(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation, const FAnimNotifyEventReference& InEventReference)
{
	Super::Notify(InMeshComp, InAnimation, InEventReference);

	if (InMeshComp)
	{
		if (ALRPlayerCharacter* OwnerCharacter = Cast<ALRPlayerCharacter>(InMeshComp->GetOwner()))
		{
			OwnerCharacter->PlayFootstepSound();
		}
	}
}

