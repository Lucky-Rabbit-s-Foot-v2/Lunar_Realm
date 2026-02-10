// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OptionManagerSubsystem.generated.h"

//============================================================================
/**
 * 옵션 관련 담당 서브시스템
 * - 옵션 카테고리별 관리
 */
 //============================================================================
 // (260210) PJB 제작. 제반 사항 구현.
 //============================================================================

UCLASS()
class LUNAR_REALM_API UOptionManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void SaveOverallSettings();
	void LoadOverallSettings();
	void SetOverallSettings();
};
