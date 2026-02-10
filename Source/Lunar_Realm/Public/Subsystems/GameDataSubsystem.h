// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/CurveTable.h"
#include "Data/LRDataStructs.h" //자체 데이터 구조체 사용
#include "GameDataSubsystem.generated.h"

/**
 * 게임 정적 데이터 관리 서브시스템
 * 
 * 주요 기능:
* - DataTable/CurveTable 로드 및 캐싱
 * - 캐릭터/장비 정적 데이터 조회
 * - 레벨별 스탯 계산 (베이스 스탯 * 캐릭터 승수)
 * - 도감 UI를 위한 데이터 제공
 * - 전역 접근으로 공개 인터페이스 활용용도.
 */
//=============================================================================
// (260128) KHS 제작. 제반 사항 구현. 
// (260128) KHS 내부 헬퍼가 많아 인터페이스 순서를 public->protected->private순으로 변경
// (260205) KHS 에너미 데이터 처리 핸들러 추가. ID파싱-> 구조체 생성자로 이전.
// (260206) KHS 데이터 테이블 소프트 레퍼런스들은 LRGameDataConfig통해 비동기 로드방식으로 변경
// (260208) KWB 스테이지 데이터 관련 항목 추가 : EnemySpawner에서 필요
// (260208) KWB 에너미 스킬 데이터 조회 함수 추가
// =============================================================================

UCLASS()
class LUNAR_REALM_API UGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// ========================================
	// 캐릭터 데이터 조회
	// ========================================
	// 캐릭터 정적 데이터 가져오기 (이름, 설명, 텍스처 등)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	const FCharacterStaticData& GetCharacterStaticData(FName CharacterID) const;
	
	// 특정 레벨의 캐릭터 최종 스탯 계산 (공식: 베이스 스탯(레벨) * 캐릭터 승수)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	float GetCharacterFinalStat(FName CharacterID, ELRStatusType StatusType, int32 CharacterLevel);
	
	//도감에 등록된 모든 캐릭터 ID 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	TArray<FName> GetAllCharacterIDs();
	
	// ========================================
	// 장비 데이터 조회
	// ========================================
	// 장비 정적 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	const FEquipmentStaticData& GetEquipmentStaticData(FName EquipmentID) const;
	
	//착용 장비 스탯 보너스 합계
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	float GetTotalEquipmentBonus(const TArray<FName>& EquipmentIDs, const TArray<int32>& EquipmentLevels, ELRStatusType StatusType);
	
	//도감에 등록된 모든 장비 ID 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	TArray<FName> GetAllEquipmentIDs();
	
	// ========================================
	// 장비 세트효과 조회
	// ========================================
	//장비 세트효과 보너스 계산
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	void GetSetEffectStatBonus(const TArray<FName>& EquipmentIDs, float& OutHPBonus, float& OutAtkBonus, float& OutDefBonus);
	
	// ========================================
	// 스킬 데이터 조회
	// ========================================
	//스킬 정적 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	const FSkillStaticData& GetSkillStaticData(FName SkillID) const;
	
	// 현재 캐릭터가 보유한 스킬 ID 조회
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	TArray<FName> GetCharacterSkillIDs(FName CharacterID);

	// 현재 에너미가 보유한 스킬 ID 조회
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	TArray<FName> GetEnemySkillIDs(FName CharacterID);

	// 현재 착용장비가 보유한 스킬 ID 조회
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	TArray<FName> GetEquipmentSkillIDs(FName EquipmentID);
	
	// ========================================
	// 에너미 데이터 조회
	// ========================================
	// 에너미 정적 데이터 가져오기 (이름, 설명, 텍스처 등)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy")
	const FEnemyStaticData& GetEnemyStaticData(FName EnemyID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy")
	TArray<FName> GetAllEnemyIDs();
	
	// ========================================
	// 스테이지 데이터 조회
	// ========================================
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Stage")
	const FStageStaticData& GetStageStaticData(FName StageID) const;

private:
	// ========================================
	// 내부 헬퍼 함수
	// ========================================
	//DataTable 로드 (Initialize 시 호출)
	void LoadDataTables();
	//초기 데이터 캐싱
	void CacheAllData();
	
	// 베이스 스탯 가져오기 (CurveTable에서 레벨에 따른 값)
	float GetBaseStatAtLevel(ELRStatusType StatusType, int32 Level);
	// 스테이터스 승수값 가져오기 (캐싱 없음)
	float GetStatusMultiplier(FName CharacterID, ELRStatusType StatusType);
	// 착용 장비 보너스 데이터 가져오기
	const FEquipmentBonus& GetEquipmentBonus(FName EquipmentID) const;
	// 착용 장비 스탯 보너스값 가져오기
	float GetEquipmentStatBonus(FName EquipmentID, ELRStatusType StatusType, int32 EquipmentLevel);
	// 착용 장비 세트효과 데이터 조회
	const FSetEffectData& GetSetEffectData(FName SetID) const;
	//활성화된 세트 ID 체크
	TArray<FName> CheckActiveSetIDs(const TArray<FName>& EquipmentIDs) const;
	
	//Enum->FName 변환 헬퍼(CT 접근용)
	static FName StatTypeToName(ELRStatusType StatusType);
	//Enum->FName 변환 헬퍼(세트 아이템 체크용
	static FName SetTypeToName(ELRSetItemType SetType);
	
	//데이터 테이블 로드 헬퍼 탬플릿
	template<typename T, typename E>
	void LoadDataTable(TSoftObjectPtr<E>& SoftTablePtr, T*& OutLoadedTable, const FString& TableName);
	
	//테이블 데이터 캐싱 헬퍼 탬플릿
	template<typename T, typename E>
	void CacheDataTable(UDataTable* DataTable, TMap<E, T>& OutRef, E T::* KeyField, const FString& TableName );
	
	//캐시데이터 조회 헬퍼 탬플릿
	template<typename T>
	const T& GetCachedData(const TMap<FName, T>& Cache, FName ID, const T& EmptyData, const TCHAR* DataName) const;

	
private:
	// ========================================
	// 로드 (Initialize 시점에 DataConfig통해 로드)
	// ========================================
	//CurveTable 캐시
	UPROPERTY()
	UCurveTable* LoadedBaseStatsCurve;
	//DataTable 캐시
	UPROPERTY()
	UDataTable* LoadedCharacterStaticData;
	UPROPERTY()
	UDataTable* LoadedCharacterMultipliers;
	UPROPERTY()
	UDataTable* LoadedEquipmentStaticData;
	UPROPERTY()
	UDataTable* LoadedEquipmentStatBonus;
	UPROPERTY()
	UDataTable* LoadedSetEffectBonus;
	UPROPERTY()
	UDataTable* LoadedSkillStaticData;
	UPROPERTY()
	UDataTable* LoadedEnemyStaticData;
	UPROPERTY()
	UDataTable* LoadedStageStaticData;
	
	// ========================================
	// 데이터 캐싱 (성능 최적화)
	// ========================================
	//캐릭터 정적 데이터 캐시
	UPROPERTY()
	TMap<FName, FCharacterStaticData> CachedCharacterStaticData;
	//장비 정적 데이터 캐시
	UPROPERTY()
	TMap<FName, FEquipmentStaticData> CachedEquipmentStaticData;
	//장비 가산 스탯보너스 캐시
	UPROPERTY()
	TMap<FName, FEquipmentBonus> CachedEquipmentBonus;
	//세트장비 가산 스탯보너스 캐시
	UPROPERTY()
	TMap<FName, FSetEffectData> CachedSetEffectData;
	//캐릭터/장비 스킬데이터 캐시
	TMap<FName, FSkillStaticData> CachedSkillStaticData;
	//에너미 정적 데이터 캐시
	TMap<FName, FEnemyStaticData> CachedEnemyStaticData;
	//스테이지 정적 데이터 캐시
	TMap<FName, FStageStaticData> CachedStageStaticData;
	
	//캐싱 실패시 사용할 기본값
	static FCharacterStaticData EmptyCharacterStaticData;
	static FEquipmentStaticData EmptyEquipmentStaticData;
	static FEquipmentBonus EmptyEquipmentBonus;
	static FSetEffectData EmptySetEffectData;
	static FSkillStaticData EmptySkillStaticData;
	static FEnemyStaticData EmptyEnemyStaticData;
	static FStageStaticData EmptyStageStaticData;
	
	
};


// ========================================
// 탬플릿 함수 구현부
// ========================================

//데이터 테이블 로드 헬퍼 탬플릿
template<typename T, typename E>
void UGameDataSubsystem::LoadDataTable(TSoftObjectPtr<E>& SoftTablePtr, T*& OutLoadedTable, const FString& TableName)
{
	if (SoftTablePtr.IsNull())
	{
		LR_WARN(TEXT("%s path is null"), *TableName);
		OutLoadedTable = nullptr;
		return;
	}
		
	//로드 시도
	OutLoadedTable = Cast<T>(SoftTablePtr.LoadSynchronous());
		
	if (OutLoadedTable)
	{
		LR_INFO(TEXT("%s load success"), *TableName);
	}
	else
	{
		LR_WARN(TEXT("Failed to load %s"),*TableName);
	}
}


//테이블 데이터 캐싱 헬퍼 탬플릿
template<typename T, typename E>
void UGameDataSubsystem::CacheDataTable(UDataTable* DataTable, TMap<E, T>& OutRef, E T::* KeyField, const FString& TableName )
{
	if (!DataTable)
	{
		LR_WARN(TEXT("DataTable is null"));
		return;
	}
	//기존 데이터 초기화
	OutRef.Empty();
	//테이블 모든 행 데이터 가져오기
	TArray<FName> rowNames = DataTable->GetRowNames();
		
	for (const FName& name : rowNames)
	{
		//DT에서 T타입 Row 추출
		if (T* row  = DataTable->FindRow<T>(name, TEXT("")))
		{
			//T 구조체 내부의 keyField 값 가져오기
			E keyValue = (*row).*KeyField;
			//맵에 추가
			OutRef.Add(keyValue, *row);
		}
	}
	LR_INFO(TEXT("Cached %d data to %s"), OutRef.Num(), *TableName);
}


//캐시데이터 조회 헬퍼 탬플릿
template<typename T>
const T& UGameDataSubsystem::GetCachedData(const TMap<FName, T>& Cache, FName ID, const T& EmptyData, const TCHAR* DataName) const
{
	const T* found = Cache.Find(ID);
	if (!found)
	{
		LR_WARN(TEXT("%s not found for ID : %s"), DataName, *ID.ToString());
		return EmptyData;
	}
		
	return *found;
}