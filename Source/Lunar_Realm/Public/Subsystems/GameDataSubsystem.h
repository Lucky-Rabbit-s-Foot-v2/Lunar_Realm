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
	const FCharacterStaticData& GetCharacterStaticData(int32 CharacterID) const;
	
	// 특정 레벨의 캐릭터 최종 스탯 계산 (공식: 베이스 스탯(레벨) * 캐릭터 승수)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	float GetCharacterFinalStat(int32 CharacterID, ELRStatusType StatusType, int32 CharacterLevel);
	
	//도감에 등록된 모든 캐릭터 ID 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	TArray<int32> GetAllCharacterIDs();
	
	// ========================================
	// 장비 데이터 조회
	// ========================================
	// 장비 정적 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	const FEquipmentStaticData& GetEquipmentStaticData(int32 EquipmentID) const;
	
	//착용 장비 스탯 보너스 합계
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	float GetTotalEquipmentBonus(const TArray<int32>& EquipmentIDs, const TArray<int32>& EquipmentLevels, ELRStatusType StatusType);
	
	//도감에 등록된 모든 장비 ID 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	TArray<int32> GetAllEquipmentIDs();
	
	// ========================================
	// 장비 세트효과 조회
	// ========================================
	//장비 세트효과 보너스 계산
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	void GetSetEffectStatBonus(const TArray<int32>& EquipmentIDs, float& OutHPBonus, float& OutAtkBonus, float& OutDefBonus);
	
	// ========================================
	// 스킬 데이터 조회
	// ========================================
	//스킬 정적 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	const FSkillStaticData& GetSkillStaticData(int32 SkillID) const;
	
	// 현재 캐릭터가 보유한 스킬 ID 조회
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	TArray<int32> GetCharacterSkillIDs(int32 CharacterID);
	
	// 현재 착용장비가 보유한 스킬 ID 조회
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	TArray<int32> GetEquipmentSkillIDs(int32 EquipmentID);
	
	// ========================================
	// 에너미 데이터 조회
	// ========================================
	// 에너미 정적 데이터 가져오기 (이름, 설명, 텍스처 등)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy")
	const FEnemyStaticData& GetEnemyStaticData(int32 EnemyID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy")
	TArray<int32> GetAllEnemyIDs();
	
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
	// 캐싱된 캐릭터 승수 데이터 가져오기 
	const FCharacterMultipliers& GetCharacterMultipliers(int32 CharacterID) const;
	// 스테이터스 승수값 가져오기 (캐싱 없음)
	float GetStatusMultiplier(int32 CharacterID, ELRStatusType StatusType);
	// 착용 장비 보너스 데이터 가져오기
	const FEquipmentBonus& GetEquipmentBonus(int32 EquipmentID) const;
	// 착용 장비 스탯 보너스값 가져오기
	float GetEquipmentStatBonus(int32 EquipmentID, ELRStatusType StatusType, int32 EquipmentLevel);
	// 착용 장비 세트효과 데이터 조회
	const FSetEffectData& GetSetEffectData(int32 SetID) const;
	//활성화된 세트 ID 체크
	TArray<int32> CheckActiveSetIDs(const TArray<int32>& EquipmentIDs) const;
	
	//Enum->FName 변환 헬퍼(CT 접근용)
	static FName StatTypeToName(ELRStatusType StatusType);
	
	//데이터 테이블 로드 헬퍼 탬플릿
	template<typename T, typename E>
	void LoadDataTable(TSoftObjectPtr<E>& SoftTablePtr, T*& OutLoadedTable, const FString& TableName);
	
	//테이블 데이터 캐싱 헬퍼 탬플릿
	template<typename T, typename E>
	void CacheDataTable(UDataTable* DataTable, TMap<E, T>& OutRef, E T::* KeyField, const FString& TableName );
	
	//캐시데이터 조회 헬퍼 탬플릿
	template<typename T>
	const T& GetCachedData(const TMap<int32, T>& Cache, int32 ID, const T& EmptyData, const TCHAR* DataName) const;

	
protected:
	// ========================================
	// DataTable 참조 (에디터에서 설정용 소프트 레퍼런스)
	// ========================================
	//공통 베이스 스탯 CurveTable - 모든 캐릭터가 공유
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UCurveTable> BaseStatsCurveTable;
	
	// 캐릭터 도감 DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> CharacterStaticDataTable;
    
	// 캐릭터별 승수 DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> CharacterMultipliersTable;
	
	// 장비 도감 DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> EquipmentStaticDataTable;
    
	// 장비스탯 보너스 DataTable 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> EquipmentStatBonusTable;

	// 세트장비 데이터 DataTable 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> EquipmentSetEffectTable;
	
	// 스킬 정적 데이터 DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> SkillStaticDataTable;

	// 에너미 정적 데이터 DataTable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|GameData|Tables")
	TSoftObjectPtr<UDataTable> EnemyStaticDataTable;
	
	
private:
	// ========================================
	// 캐싱 (성능 최적화. Initialize 시점에 모두 로드)
	// ========================================
	//캐릭터 정적 데이터 캐시
	UPROPERTY()
	TMap<int32, FCharacterStaticData> CachedCharacterStaticData;
	//캐릭터 승수 캐시
	UPROPERTY()
	TMap<int32, FCharacterMultipliers> CachedCharacterMultipliers;
	//장비 정적 데이터 캐시
	UPROPERTY()
	TMap<int32, FEquipmentStaticData> CachedEquipmentStaticData;
	//장비 가산 스탯보너스 캐시
	UPROPERTY()
	TMap<int32, FEquipmentBonus> CachedEquipmentBonus;
	//세트장비 가산 스탯보너스 캐시
	UPROPERTY()
	TMap<int32, FSetEffectData> CachedSetEffectData;
	//캐릭터/장비 스킬데이터 캐시
	TMap<int32, FSkillStaticData> CachedSkillStaticData;
	//에너미 정적 데이터 캐시
	TMap<int32, FEnemyStaticData> CachedEnemyStaticData;
	
	//캐싱 실패시 사용할 기본값
	static FCharacterStaticData EmptyCharacterStaticData;
	static FCharacterMultipliers EmptyCharacterMultipliers;
	static FEquipmentStaticData EmptyEquipmentStaticData;
	static FEquipmentBonus EmptyEquipmentBonus;
	static FSetEffectData EmptySetEffectData;
	static FSkillStaticData EmptySkillStaticData;
	static FEnemyStaticData EmptyEnemyStaticData;
	
	// ========================================
	// 커브/데이터 테이블 참조 캐시
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
const T& UGameDataSubsystem::GetCachedData(const TMap<int32, T>& Cache, int32 ID, const T& EmptyData, const TCHAR* DataName) const
{
	const T* found = Cache.Find(ID);
	if (!found)
	{
		LR_WARN(TEXT("%s not found for ID : %d"), DataName, ID);
		return EmptyData;
	}
		
	return *found;
}