// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/LRInputConfig.h"

const UInputAction* ULRInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FLRInputAction& Action : AbilityInputActions)
	{
		// 태그가 일치하는 액션을 찾아서 리턴
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s], on InputConfig [%s]"), *InputTag.ToString(), *GetName());
	}

	return nullptr;
}
