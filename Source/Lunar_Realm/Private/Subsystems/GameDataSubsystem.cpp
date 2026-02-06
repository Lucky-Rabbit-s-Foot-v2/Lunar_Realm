// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameDataSubsystem.h"
#include "Data/LRGameDataConfig.h"

//Static 기본값 정의(조회 실패시 반환용도)
FCharacterStaticData UGameDataSubsystem::EmptyCharacterStaticData;
FCharacterMultipliers UGameDataSubsystem::EmptyCharacterMultipliers;
FEquipmentStaticData UGameDataSubsystem::EmptyEquipmentStaticData;
FEquipmentBonus UGameDataSubsystem::EmptyEquipmentBonus;
FSetEffectData UGameDataSubsystem::EmptySetEffectData;
FSkillStaticData UGameDataSubsystem::EmptySkillStaticData;
FEnemyStaticData UGameDataSubsystem::EmptyEnemyStaticData;


void UGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	//데이터테이블 로드
	LoadDataTables();
	//초기화 시점 캐싱
	CacheAllData();
	
	LR_INFO(TEXT("GameDataSubsystem is initialized"));
}

void UGameDataSubsystem::Deinitialize()
{
	CachedCharacterStaticData.Empty();
	CachedEquipmentStaticData.Empty();
	CachedCharacterMultipliers.Empty();
	CachedEquipmentBonus.Empty();
	CachedSetEffectData.Empty();
	CachedSkillStaticData.Empty();
	CachedEnemyStaticData.Empty();
	
	LR_INFO(TEXT("GameDataSubsystem Deinitialize - Cleaned up caches"));
	
	Super::Deinitialize();
}

// ========================================
// 내부 헬퍼 함수(private)
// ========================================

float UGameDataSubsystem::GetBaseStatAtLevel(ELRStatusType StatusType, int32 Level)
{
	if (!ensureMsgf(LoadedBaseStatsCurve, TEXT("BaseStatsCurveTable is not loaded")))
	{
		return 0.f;
	}
	
	FName curveName = StatTypeToName(StatusType);
	FRealCurve* curve = LoadedBaseStatsCurve->FindCurve(curveName, TEXT(""), false);
	if (curve)
	{
		float value = curve->Eval(static_cast<float>(Level));
		return value;
	}
	
	LR_WARN(TEXT("Curve %s is not found in BaseStatsCurveTable"), *curveName.ToString());
	return 0.f;
	
}

float UGameDataSubsystem::GetStatusMultiplier(int32 CharacterID, ELRStatusType StatusType)
{
	FCharacterMultipliers multipliers = GetCharacterMultipliers(CharacterID);
	
	//스탯 이름에 따라 승수 반환
	switch (StatusType)
	{
	case ELRStatusType::HP: return multipliers.HPMultiplier;
	case ELRStatusType::ATK: return multipliers.AttackMultiplier;
	case ELRStatusType::DEF : return multipliers.DefenseMultiplier;
	default:
		{
			LR_WARN(TEXT("Invalid EnumType"));
			return 1.0f;
		}
	}
}

void UGameDataSubsystem::LoadDataTables()
{
	// Config 로드
	ULRGameDataConfig* Config = LoadObject<ULRGameDataConfig>(
		nullptr, TEXT("/Script/Lunar_Realm.LRGameDataConfig'/Game/DataTables/LRGameDataConfig.LRGameDataConfig'"));
    
	if (!ensureMsgf(Config,TEXT("FAILED TO LOAD GameDataConfig!!!!!")))
	{
		return;
	}
	
	// Config의 SoftObjectPtr를 경유하여 로드 → LoadedXXX에 저장
	LoadedBaseStatsCurve = Config->BaseStatsCurveTable.LoadSynchronous(); //베이스 스탯 커브
	LoadedCharacterStaticData = Config->CharacterStaticDataTable.LoadSynchronous(); //캐릭터 데이터
	LoadedCharacterMultipliers = Config->CharacterMultipliersTable.LoadSynchronous(); //캐릭터 승수
	LoadedEnemyStaticData = Config->EnemyStaticDataTable.LoadSynchronous(); //적 데이터
	LoadedEquipmentStaticData = Config->EquipmentStaticDataTable.LoadSynchronous(); //장비 데이터
	LoadedEquipmentStatBonus = Config->EquipmentStatBonusTable.LoadSynchronous(); //장비 보너스
	LoadedSetEffectBonus = Config->EquipmentSetEffectTable.LoadSynchronous(); //세트장비 효과
	LoadedSkillStaticData = Config->SkillStaticDataTable.LoadSynchronous(); //스킬 데이터
	
	//로직 변경으로 시스템에서 직접 로드 방식은 미사용
	// LoadDataTable(BaseStatsCurveTable, LoadedBaseStatsCurve, TEXT("BaseStatsCurveTable"));
	// LoadDataTable(CharacterStaticDataTable, LoadedCharacterStaticData, TEXT("CharacterStaticData"));
	// LoadDataTable(CharacterMultipliersTable, LoadedCharacterMultipliers, TEXT("CharacterMultipliers"));
	// LoadDataTable(EquipmentStaticDataTable, LoadedEquipmentStaticData, TEXT("EquipmentStaticData"));
	// LoadDataTable(EquipmentStatBonusTable, LoadedEquipmentStatBonus, TEXT("EquipmentStatBonus"));
	// LoadDataTable(EquipmentSetEffectTable, LoadedSetEffectBonus, TEXT("SetEffectData"));
	// LoadDataTable(SkillStaticDataTable, LoadedSkillStaticData, TEXT("SkillStaticData"));
	// LoadDataTable(EnemyStaticDataTable, LoadedEnemyStaticData, TEXT("EnemyStaticData"));
}

void UGameDataSubsystem::CacheAllData()
{
	//ID기반으로 로드된 정보를 캐싱 데이터에 저장.
	//캐릭터 정적데이터 캐싱 
	CacheDataTable<FCharacterStaticData, int32>(
		LoadedCharacterStaticData, CachedCharacterStaticData, &FCharacterStaticData::CharacterID, TEXT("CharacterStaticData"));
	//캐릭터 승수데이터 캐싱
	CacheDataTable<FCharacterMultipliers, int32>(
		LoadedCharacterMultipliers, CachedCharacterMultipliers, &FCharacterMultipliers::CharacterID, TEXT("CharacterMultipliers"));
	
	//장비 정적데이터 캐싱
	CacheDataTable<FEquipmentStaticData, int32>(
		LoadedEquipmentStaticData, CachedEquipmentStaticData, &FEquipmentStaticData::EquipmentID, TEXT("EquipmentStaticData"));
	//장비 스탯 보너스 데이터 캐싱
	CacheDataTable<FEquipmentBonus, int32>(
		LoadedEquipmentStatBonus, CachedEquipmentBonus, &FEquipmentBonus::EquipmentID, TEXT("EquipmentBonus"));
	
	//세트 장비 효과 데이터 캐싱
	CacheDataTable<FSetEffectData, int32>(
		LoadedSetEffectBonus, CachedSetEffectData, &FSetEffectData::SetID, TEXT("SetEffectData"));
	
	//스킬 데이터 캐싱
	CacheDataTable<FSkillStaticData, int32>(
		LoadedSkillStaticData, CachedSkillStaticData, &FSkillStaticData::SkillID, TEXT("SkillStaticData"));
	
	//에너미 데이터 캐싱
	CacheDataTable<FEnemyStaticData, int32>(
		LoadedEnemyStaticData, CachedEnemyStaticData, &FEnemyStaticData::CharacterID, TEXT("EnemyStaticData"));
}

FName UGameDataSubsystem::StatTypeToName(ELRStatusType StatusType)
{
	switch (StatusType)
	{
	case ELRStatusType::HP : return FName(TEXT("HP")); 
	case ELRStatusType::ATK: return FName(TEXT("Attack"));
	case ELRStatusType::DEF : return FName(TEXT("Defense"));
	
	default:
		{
			LR_WARN(TEXT("Unknown StatusType"));
			return FName(TEXT("HP"));
		}
	}
}

// ========================================
// 캐릭터 데이터 조회
// ========================================

const FCharacterStaticData& UGameDataSubsystem::GetCharacterStaticData(int32 CharacterID) const
{
	return GetCachedData(CachedCharacterStaticData, CharacterID, EmptyCharacterStaticData, TEXT("CharacterStaticData"));
}


const FCharacterMultipliers& UGameDataSubsystem::GetCharacterMultipliers(int32 CharacterID) const
{
	return GetCachedData(CachedCharacterMultipliers, CharacterID, EmptyCharacterMultipliers, TEXT("CharacterMultipliers"));
}

float UGameDataSubsystem::GetCharacterFinalStat(int32 CharacterID, ELRStatusType StatusType, int32 CharacterLevel)
{
	float baseStat = GetBaseStatAtLevel(StatusType, CharacterLevel); //베이스 스탯
	float multiplier = GetStatusMultiplier(CharacterID, StatusType); //스탯 승수
	float finalStat = baseStat * multiplier; //최종 스탯
	
	LR_INFO(TEXT("CharacterID : %d, Stat : %s, Level : %d, Base = %.1f * Mult = %.2f = %.1f"), 
		CharacterID, *StatTypeToName(StatusType).ToString(), CharacterLevel, baseStat, multiplier, finalStat);
	
	return finalStat;
}


TArray<int32> UGameDataSubsystem::GetAllCharacterIDs()
{
	TArray<int32> characterIDs;
	CachedCharacterStaticData.GetKeys(characterIDs);
	characterIDs.Sort();
	
	LR_INFO(TEXT("Found %d Characters"), characterIDs.Num());
	
	return characterIDs;
}

// ========================================
// 장비 데이터 조회
// ========================================
const FEquipmentStaticData& UGameDataSubsystem::GetEquipmentStaticData(int32 EquipmentID) const
{
	return GetCachedData(CachedEquipmentStaticData, EquipmentID, EmptyEquipmentStaticData, TEXT("EquipmentStaticData"));
}

const FEquipmentBonus& UGameDataSubsystem::GetEquipmentBonus(int32 EquipmentID) const
{
	return GetCachedData(CachedEquipmentBonus, EquipmentID, EmptyEquipmentBonus, TEXT("EquipmentBonus"));
}

float UGameDataSubsystem::GetEquipmentStatBonus(int32 EquipmentID, ELRStatusType StatusType, int32 EquipmentLevel)
{
	const FEquipmentBonus& bonus = GetEquipmentBonus(EquipmentID);
	
	float baseAmount = 0.f;
	float bonusPerLevel = 0.f;
	
	switch (StatusType)
	{
	case ELRStatusType::HP:
		{
			baseAmount = bonus.BaseHPBonus;
			bonusPerLevel = bonus.HPBonusPerLevel;
			break;
		}
	case ELRStatusType::ATK:
		{
			baseAmount = bonus.BaseAttackBonus;
			bonusPerLevel = bonus.AttackBonusPerLevel;
			break;
		}
	case ELRStatusType::DEF:
		{
			baseAmount = bonus.BaseDefenseBonus;
			bonusPerLevel = bonus.DefenseBonusPerLevel;
			break;
		}
		
	default:
		{
			LR_WARN(TEXT("Invalid Enumtype"));
			break;
		}
	}
	
	//공식 : 기본 보너스 + (레벨 - 1) * 레벨당 보너스
	return baseAmount + (EquipmentLevel - 1) * bonusPerLevel;
}

float UGameDataSubsystem::GetTotalEquipmentBonus(const TArray<int32>& EquipmentIDs,
	const TArray<int32>& EquipmentLevels, ELRStatusType StatusType)
{
	float totalBonus = 0.f;
	int32 count = FMath::Min(EquipmentIDs.Num(), EquipmentLevels.Num()); //유효한 갯수만.
	
	for (int i = 0; i < count; i++)
	{
		if (EquipmentIDs[i] > 0)
		{
			totalBonus += GetEquipmentStatBonus(EquipmentIDs[i], StatusType, EquipmentLevels[i]);
		}
	}
	
	return totalBonus;
}

TArray<int32> UGameDataSubsystem::GetAllEquipmentIDs()
{
	TArray<int32> equipmentIDs;
	CachedEquipmentStaticData.GetKeys(equipmentIDs);
	equipmentIDs.Sort();
	
	LR_INFO(TEXT("Found %d Equipments"), equipmentIDs.Num());
	
	return equipmentIDs;
}


// ========================================
// 세트 장비 효과 데이터 조회
// ========================================
const FSetEffectData& UGameDataSubsystem::GetSetEffectData(int32 SetID) const
{
	return GetCachedData(CachedSetEffectData, SetID, EmptySetEffectData, TEXT("SetEffectData"));
}


TArray<int32> UGameDataSubsystem::CheckActiveSetIDs(const TArray<int32>& EquipmentIDs) const
{
	//세트별 장착갯수 집계
	TMap<ELRSetItemType, int32> setCounts; 
	
	for (int32 id : EquipmentIDs)
	{
		if (id <= 0)
		{
			continue;
		}
		
		FEntityIDInfo info(id);
		ELRSetItemType setType = info.GetSetItemType();
		
		if (setType != ELRSetItemType::NONE) //기본 제외
		{
			setCounts.FindOrAdd(setType)++;
		}
	}
	
	//활성화 조건 체크
	TArray<int32> activeSets;
	
	for (auto& [setType, count] : setCounts)
	{
		int32 setId = static_cast<int32>(setType);
		//세트 달성여부 확인
		const FSetEffectData& setData = GetSetEffectData(setId);
		
		if (count >= setData.RequiredPieces)
		{
			activeSets.Add(setId);
			LR_INFO(TEXT("Set %d activated : %d/%d pieces"), setId, count, setData.RequiredPieces);
		}
	}
	
	return activeSets;
}

void UGameDataSubsystem::GetSetEffectStatBonus(const TArray<int32>& EquipmentIDs, float& OutHPBonus, float& OutAtkBonus,
	float& OutDefBonus)
{
	OutHPBonus = 1.f;
	OutAtkBonus = 1.f;
	OutDefBonus = 1.f;
	
	//세트효과 활성화 체크
	TArray<int32> activeSets = CheckActiveSetIDs(EquipmentIDs);
	
	for (int32 setID : activeSets)
	{
		FSetEffectData data = GetSetEffectData(setID);
		
		if (data.BonusHPPercent <= 0 || data.BonusAttackPercent <= 0 || data.BonusDefensePercent <= 0)
		{
			continue;
		}
		
		//보너스 승수 계산
		OutHPBonus *= data.BonusHPPercent;
		OutAtkBonus *= data.BonusAttackPercent;
		OutDefBonus *= data.BonusDefensePercent;
		
		LR_INFO(TEXT("get bonus from set equipment(%d) : HP * %.0f, ATK * %0.f, DEF * %0.f"), 
			setID, data.BonusHPPercent, data.BonusAttackPercent, data.BonusDefensePercent);
	}
}



// ========================================
// 스킬 데이터 조회
// ========================================
const FSkillStaticData& UGameDataSubsystem::GetSkillStaticData(int32 SkillID) const
{
	return GetCachedData(CachedSkillStaticData, SkillID, EmptySkillStaticData, TEXT("SkillStaticData"));
}


TArray<int32> UGameDataSubsystem::GetCharacterSkillIDs(int32 CharacterID)
{
	FCharacterStaticData data = GetCharacterStaticData(CharacterID);
	
	return data.SkillIDs;
}

TArray<int32> UGameDataSubsystem::GetEquipmentSkillIDs(int32 EquipmentID)
{
	FEquipmentStaticData data = GetEquipmentStaticData(EquipmentID);
	
	return data.SkillIDs;
}

const FEnemyStaticData& UGameDataSubsystem::GetEnemyStaticData(int32 EnemyID) const
{
	return GetCachedData(CachedEnemyStaticData, EnemyID, EmptyEnemyStaticData, TEXT("EnemyStaticData"));
}

TArray<int32> UGameDataSubsystem::GetAllEnemyIDs()
{
	TArray<int32> EnemyIDs;
	CachedEnemyStaticData.GetKeys(EnemyIDs);
	EnemyIDs.Sort();
	
	LR_INFO(TEXT("Found %d Enemy"), EnemyIDs.Num());
	
	return EnemyIDs;
}
