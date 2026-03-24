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
// (260224) KHS GA필요 데이터 추가.
// (260225) PJB 챕터로 스테이지 불러오기
// (260226) KHS 스킬 기획 변경에 따른 헬퍼함수 추가.
// (260318) BJM 캐릭터 사운드 데이터 처리 핸들러 추가.
// (260320) BJM 스킬 아이콘 데이터 헬퍼함수 추가
// (260322) KWB 에너미 사운드 데이터 처리 핸들러 추가.
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
	// 캐릭터 사운드 데이터 조회(260318 BJM)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Sound")
	const FCharacterSoundData& GetCharacterSoundData(FName CharacterName) const;
	
	// 특정 레벨의 캐릭터 최종 스탯 계산 (공식: 베이스 스탯(레벨) * 캐릭터 승수)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	float GetCharacterFinalStat(FName CharacterID, ELRStatusType StatusType, int32 CharacterLevel);
	
	//모든 캐릭터 ID 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Character")
	TArray<FName> GetAllCharacterIDs();

	
	// ========================================
	// 장비 정적 데이터 조회
	// ========================================
	// 장비 정적 데이터 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	const FEquipmentStaticData& GetEquipmentStaticData(FName EquipmentID) const;
	
	//착용 장비 스탯 보너스 합계
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	float GetTotalEquipmentBonus(const TArray<FName>& EquipmentIDs, const TArray<int32>& EquipmentLevels, ELRStatusType StatusType);
	
	//모든 장비 ID 가져오기
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	TArray<FName> GetAllEquipmentIDs();
	
	// ========================================
	// 장비 세트효과 정적 데이터 조회
	// ========================================
	//장비 세트효과 보너스 계산
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Equipment")
	void GetSetEffectStatBonus(const TArray<FName>& EquipmentIDs, float& OutHPBonus, float& OutAtkBonus, float& OutDefBonus);
	
	// ========================================
	// 스킬 정적 데이터 조회
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

	// 스킬 아이콘 텍스처 가져오기 (260320 BJM)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Skills")
	TSoftObjectPtr<UTexture2D> GetSkillIcon(FName InSkillID) const;
	
	// ========================================
	// GA 데이터 조회
	// ========================================
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Abilites")
	const FSkillEffectData& GetSkillEffectData(FName SkillEffectID) const;
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Ability")
	const FSkillResourceData& GetSkillResourceData(FName ResourceID) const;
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Ability")
	const FSkillSpawnData& GetSkillSpawnData(FName SkillEffectID) const;
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Abilites")
	const FSkillHitAreaData& GetSkillHitAreaData(FName SkillEffectID) const;
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Abilites")
	const FStatusEffectData& GetStatusEffectData(FName StatusEffectID) const;
	
	// FlightType 전용 템플릿 조회
	// 사용 예: GetFlightData<FFlightHomingData>(SkillEffectID)
	template<typename T>
	const T& GetFlightData(FName SkillEffectID) const;

	// 내부에서 T 타입에 맞는 캐시를 선택하는 헬퍼
	template<typename T>
	const TMap<FName, T>& GetFlightCache() const;
	
	// ========================================
	// 에너미 정적 데이터 조회
	// ========================================
	// 에너미 정적 데이터 가져오기 (이름, 설명, 텍스처 등)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy")
	const FEnemyStaticData& GetEnemyStaticData(FName EnemyID) const;
	
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy")
	TArray<FName> GetAllEnemyIDs();
	
	// 에너미 사운드 데이터 조회(260322 KWB)
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Enemy|Sound")
	const FEnemySoundData& GetEnemySoundData(FName EnemyID) const;

	// ========================================
	// 스테이지 정적 데이터 조회
	// ========================================
	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Stage")
	const FStageStaticData& GetStageStaticData(FName StageID) const;

	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Stage")
	const FChapterStaticData& GetChapterStaticData(FName ChapterID) const;

	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Stage")
	const TArray<FName> GetAllStageIDsByChapterID(FName ChapterID) const;

	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Stage")
	const TArray<FName> GetAllStageID() const;

	UFUNCTION(BlueprintCallable, Category = "LR|GameData|Currecny")
	const FCurrencyStaticData& GetCurrencyStaticData(FName CurrencyID) const;

private:
	// ========================================
	// 내부 헬퍼 함수
	// ========================================
	//DataTable 로드 (Initialize 시 호출)
	void LoadDataTables();
	//초기 데이터 캐싱
	void CacheAllData();
	
public:
	// 베이스 스탯 가져오기 (CurveTable에서 레벨에 따른 값)
	float GetBaseStatAtLevel(ELRStatusType StatusType, int32 Level);

	// 착용 장비 스탯 보너스값 가져오기
	float GetEquipmentStatBonus(FName EquipmentID, ELRStatusType StatusType, int32 EquipmentLevel);

protected:
	// 스테이터스 승수값 가져오기 (캐싱 없음)
	float GetStatusMultiplier(FName CharacterID, ELRStatusType StatusType);
	// 착용 장비 보너스 데이터 가져오기
	const FEquipmentBonus& GetEquipmentBonus(FName EquipmentID) const;
	// 착용 장비 세트효과 데이터 조회
	const FSetEffectData& GetSetEffectData(FName SetID) const;
	//활성화된 세트 ID 체크
	TArray<FName> CheckActiveSetIDs(const TArray<FName>& EquipmentIDs) const;
	//Enum->FName 변환 헬퍼(CT 접근용)
	static FName StatTypeToName(ELRStatusType StatusType);
	//Enum->FName 변환 헬퍼(세트 아이템 체크용
	static FName SetTypeToName(ELRSetItemType SetType);
	
	//FName->Enum 변환 헬퍼(DT 접근용)
	static EFlightType ParseSkillType(FName TypeName);
	static EStatusType ParseBuffType(FName TypeName);
	
	
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
	// (260318) BJM 캐릭터 사운드 데이터 추가
	UPROPERTY()
	UDataTable* LoadedCharacterSoundData;
	UPROPERTY()
	UDataTable* LoadedEquipmentStaticData;
	UPROPERTY()
	UDataTable* LoadedEquipmentStatBonus;
	UPROPERTY()
	UDataTable* LoadedSetEffectBonus;
	// (260226) KHS v1.2 신규 추가
	UPROPERTY()
	UDataTable* LoadedSkillStaticData;
	UPROPERTY()
	UDataTable* LoadedSkillEffectData;
	UPROPERTY()
	UDataTable* LoadedSkillResourceData;
	UPROPERTY()
	UDataTable* LoadedSkillSpawnData;
	UPROPERTY()
	UDataTable* LoadedSkillFlightHomingData;
	UPROPERTY()
	UDataTable* LoadedSkillFlightArcData;
	UPROPERTY()
	UDataTable* LoadedSkillFlightPierceData;
	UPROPERTY()
	UDataTable* LoadedSkillFlightExplodeData;
	UPROPERTY()
	UDataTable* LoadedSkillHitAreaData;
	UPROPERTY()
	UDataTable* LoadedStatusEffectData;
	UPROPERTY()
	UDataTable* LoadedEnemyStaticData;
	UPROPERTY()
	UDataTable* LoadedStageStaticData;
	UPROPERTY()
	UDataTable* LoadedChapterStaticData;
	UPROPERTY()
	UDataTable* LoadedCurrencyStaticData;


	// ========================================
	// 데이터 캐싱 (성능 최적화)
	// ========================================
	//캐릭터 정적 데이터 캐시
	UPROPERTY()
	TMap<FName, FCharacterStaticData> CachedCharacterStaticData;
	//(260318) BJM 캐릭터 사운드 데이터 캐시 추가
	UPROPERTY()
	TMap<FName, FCharacterSoundData> CachedCharacterSoundData;
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
	
	//스킬 GA 데이터 캐시
	UPROPERTY()
	TMap<FName, FSkillEffectData> CachedSkillEffectData;
	UPROPERTY()
	TMap<FName, FSkillResourceData> CachedSkillResourceData;
	// (260226) KHS v1.2 신규 추가
	UPROPERTY()
	TMap<FName, FSkillSpawnData> CachedSkillSpawnData;
	UPROPERTY()
	TMap<FName, FFlightHomingData> CachedFlightHomingData;
	UPROPERTY()
	TMap<FName, FFlightArcData> CachedFlightArcData;
	UPROPERTY()
	TMap<FName, FFlightPierceData> CachedFlightPierceData;
	UPROPERTY()
	TMap<FName, FFlightExplodeData> CachedFlightExplodeData;
	UPROPERTY()
	TMap<FName, FSkillHitAreaData> CachedSkillHitAreaData;
	//스킬 효과 버프/디버프 데이터 캐시
	UPROPERTY()
	TMap<FName, FStatusEffectData> CachedBuffEffectData;
	
	//에너미 정적 데이터 캐시
	UPROPERTY()
	TMap<FName, FEnemyStaticData> CachedEnemyStaticData;
	//(260323) KWB 에너미 사운드 데이터 캐시
	UPROPERTY()
	TMap<FName, FEnemySoundData> CachedEnemySoundData;
	//스테이지 정적 데이터 캐시
	UPROPERTY()
	TMap<FName, FStageStaticData> CachedStageStaticData;
	UPROPERTY()
	TMap<FName, FChapterStaticData> CachedChapterStaticData;
	UPROPERTY()
	TMap<FName, FCurrencyStaticData> CachedCurrencyStaticData;

	//캐싱 실패시 사용할 기본값
	static FCharacterStaticData EmptyCharacterStaticData;
	static FCharacterSoundData EmptyCharacterSoundData;
	static FEquipmentStaticData EmptyEquipmentStaticData;
	static FEquipmentBonus EmptyEquipmentBonus;
	static FSetEffectData EmptySetEffectData;
	static FSkillStaticData EmptySkillStaticData;
	static FSkillResourceData EmptySkillResourceData;
	static FSkillEffectData EmptySkillEffectData;
	static FSkillSpawnData EmptySkillSpawnData;
	static FFlightHomingData EmptyFlightHomingData;
	static FFlightArcData EmptyFlightArcData;
	static FFlightPierceData EmptyFlightPierceData;
	static FFlightExplodeData EmptyFlightExplodeData;
	static FSkillHitAreaData EmptySkillHitAreaData;
	static FStatusEffectData EmptyStatusEffectData;
	static FEnemyStaticData EmptyEnemyStaticData;
	static FEnemySoundData EmptyEnemySoundData;
	static FStageStaticData EmptyStageStaticData;
	static FChapterStaticData EmptyChapterStaticData;
	static FCurrencyStaticData EmptyCurrencyStaticData;

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
			
			//LR_INFO(TEXT("Row [%s] -> KeyField value [%s]"), *name.ToString(), *keyValue.ToString());
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

// ========================================
// GetFlightData 템플릿 구현부
// (260226) KHS v1.2 신규 추가
// ========================================

// T 타입에 맞는 캐시 맵을 반환하는 특수화
template<typename T>
const TMap<FName, T>& UGameDataSubsystem::GetFlightCache() const
{
	// 기본 템플릿 — 잘못된 타입 사용 시 컴파일 에러로 방어
	static_assert(sizeof(T) == 0, "GetFlightCache: 지원하지 않는 FlightData 타입입니다.");
	
	// 컴파일러 경고 억제용 더미 반환 (실제로 실행되지 않음)
	static TMap<FName, T> Dummy;
	return Dummy;
}

// FFlightHomingData 전용 특수화
template<>
inline const TMap<FName, FFlightHomingData>& UGameDataSubsystem::GetFlightCache() const
{
	return CachedFlightHomingData;
}

// FFlightArcData 전용 특수화
template<>
inline const TMap<FName, FFlightArcData>& UGameDataSubsystem::GetFlightCache() const
{
	return CachedFlightArcData;
}

// FFlightPierceData 전용 특수화
template<>
inline const TMap<FName, FFlightPierceData>& UGameDataSubsystem::GetFlightCache() const
{
	return CachedFlightPierceData;
}

// FFlightExplodeData 전용 특수화
template<>
inline const TMap<FName, FFlightExplodeData>& UGameDataSubsystem::GetFlightCache() const
{
	return CachedFlightExplodeData;
}

// 실제 조회 함수 — 위 특수화를 통해 올바른 캐시 맵 선택
template<typename T>
const T& UGameDataSubsystem::GetFlightData(FName SkillEffectID) const
{
	const TMap<FName, T>& Cache = GetFlightCache<T>();
	
	const T* Found = Cache.Find(SkillEffectID);
	if (!Found)
	{
		LR_WARN(TEXT("FlightData not found for SkillEffectID: %s"), *SkillEffectID.ToString());
		static T Empty;
		return Empty;
	}
	
	return *Found;
}

