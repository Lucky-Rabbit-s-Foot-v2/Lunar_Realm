// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/LRInputConfig.h"
#include "LRInputComponent.generated.h"


//=============================================================================
// (260204) BJM 제작. InputComponent 생성 및 입력관련 태그 관리.
//=============================================================================
/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	// LRInputConfig(DataAsset)를 받아서 자동으로 바인딩해주는 함수

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const ULRInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);

};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void ULRInputComponent::BindAbilityActions(const ULRInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(InputConfig);

	// LRInputConfig(DataAsset)에 있는 모든 액션을 하나씩 꺼내서 연결
	for (const FLRInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			// 키 눌렀을 때 (Pressed) -> 태그를 함수에 전달하며 실행
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			// 키 뗐을 때 (Released) -> 태그를 함수에 전달하며 실행 (옵션)
			if constexpr (!std::is_same_v<ReleasedFuncType, std::nullptr_t>)
			{
				if (ReleasedFunc)
				{
					BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
				}
			}
		}
	}
}
