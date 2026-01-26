// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


// ============================================
// 공통 헤더(PCH) - 프로젝트 전역에서 사용(Pre-Compile Header)
// 전체 리컴파일 방지를 위해 수정사항이 없는 core헤더 등만 포함 권장.
// (260126) KHS 내용 제작. Build.cs 수정완료.
// ============================================
// GAS 
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

// AttributeSet 관련
#include "AttributeSet.h"

// GameplayEffect 관련
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

// GameplayAbility 관련
#include "Abilities/GameplayAbility.h"

// GameplayTag 관련
#include "GameplayTagContainer.h"

// 유틸리티
#include "AbilitySystemBlueprintLibrary.h"

//로그 시스템
#include "System/LoggingSystem.h"
