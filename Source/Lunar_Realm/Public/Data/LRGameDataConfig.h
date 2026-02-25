// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Engine/CurveTable.h"
#include "LRGameDataConfig.generated.h"

// =============================================================================
/** 
 * LRGameDataConfig
 * - GameDataSubsystem이 사용할 테이블 경로 설정
 * - 에디터에서 할당하여 사용
 */
//=============================================================================
// (260206) KHS 제작. 제반 사항 구현.
// (260209) KWB 스테이지 정적 데이터 테이블 추가.
// =============================================================================
UCLASS()
class LUNAR_REALM_API ULRGameDataConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// ========================================
	// DataTable 참조 (에디터에서 설정용 소프트 레퍼런스)
	// ========================================
	//공통 베이스 스탯 CurveTable - 모든 캐릭터가 공유
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UCurveTable> BaseStatsCurveTable;
	
	// 캐릭터 도감 DataTable
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Character")
	TSoftObjectPtr<UDataTable> CharacterStaticDataTable;
	
	// 에너미 정적 데이터 DataTable
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Enemy")
	TSoftObjectPtr<UDataTable> EnemyStaticDataTable;

	
	// 장비 도감 DataTable
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Equipment")
	TSoftObjectPtr<UDataTable> EquipmentStaticDataTable;
    
	// 장비스탯 보너스 DataTable 
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Equipment")
	TSoftObjectPtr<UDataTable> EquipmentStatBonusTable;

	
	// 세트장비 데이터 DataTable 
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Equipment")
	TSoftObjectPtr<UDataTable> EquipmentSetEffectTable;
	
	
	// 스킬 정적 데이터 DataTable
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Skill")
	TSoftObjectPtr<UDataTable> SkillStaticDataTable;
	
	// 스킬효과 정적 데이터 DataTable(GA 사용) 
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Ability")
	TSoftObjectPtr<UDataTable> SkillEffectDataTable;
	
	// 스킬효과 추가 요구 파라미터 DataTable(GA 사용) 
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Ability")
	TSoftObjectPtr<UDataTable> SkillEffectParameterDataTable;
	
	// 스킬 결과 버프/디버프 효과 DataTable(GE 사용) 
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Ability")
	TSoftObjectPtr<UDataTable> BuffEffectDataTable;
	

	// 스테이지 정적 데이터 DataTable
	UPROPERTY(EditDefaultsOnly, Category = "LR|GameData|Stage")
	TSoftObjectPtr<UDataTable> StageStaticDataTable;
};
