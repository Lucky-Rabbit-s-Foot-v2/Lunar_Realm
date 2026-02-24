// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/LRAnimNotify.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

ULRAnimNotify::ULRAnimNotify()
{
}

void ULRAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AActor* OwnerActor = MeshComp->GetOwner();

		if (EventTag.IsValid())
		{
			FGameplayEventData Payload;
			Payload.Instigator = OwnerActor;
			Payload.EventTag = EventTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, Payload);
		}
	}
}
