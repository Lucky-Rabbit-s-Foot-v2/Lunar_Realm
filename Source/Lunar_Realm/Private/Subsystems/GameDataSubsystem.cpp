// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameDataSubsystem.h"
#include "Data/LRGameDataConfig.h"

//Static 기본값 정의(조회 실패시 반환용도)
FCharacterStaticData UGameDataSubsystem::EmptyCharacterStaticData;
FEquipmentStaticData UGameDataSubsystem::EmptyEquipmentStaticData;
FEquipmentBonus UGameDataSubsystem::EmptyEquipmentBonus;
FSetEffectData UGameDataSubsystem::EmptySetEffectData;
FSkillStaticData UGameDataSubsystem::EmptySkillStaticData;
FEnemyStaticData UGameDataSubsystem::EmptyEnemyStaticData;
FStageStaticData UGameDataSubsystem::EmptyStageStaticData;


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
	CachedEquipmentBonus.Empty();
	CachedSetEffectData.Empty();
	CachedSkillStaticData.Empty();
	CachedEnemyStaticData.Empty();
	CachedStageStaticData.Empty();
	
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

float UGameDataSubsystem::GetStatusMultiplier(FName CharacterID, ELRStatusType StatusType)
{
	FCharacterStaticData data = GetCharacterStaticData(CharacterID);
	
	//스탯 이름에 따라 승수 반환
	switch (StatusType)
	{
	case ELRStatusType::HP: return data.HPMultiplier;
	case ELRStatusType::ATK: return data.AttackMultiplier;
	case ELRStatusType::DEF : return data.DefenseMultiplier;
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
	LoadedEnemyStaticData = Config->EnemyStaticDataTable.LoadSynchronous(); //적 데이터
	LoadedEquipmentStaticData = Config->EquipmentStaticDataTable.LoadSynchronous(); //장비 데이터
	LoadedEquipmentStatBonus = Config->EquipmentStatBonusTable.LoadSynchronous(); //장비 보너스
	LoadedSetEffectBonus = Config->EquipmentSetEffectTable.LoadSynchronous(); //세트장비 효과
	LoadedSkillStaticData = Config->SkillStaticDataTable.LoadSynchronous(); //스킬 데이터
	LoadedStageStaticData = Config->StageStaticDataTable.LoadSynchronous(); //스테이지 데이터
	
	//로직 변경으로 시스템에서 직접 로드 방식은 미사용
	// LoadDataTable(BaseStatsCurveTable, LoadedBaseStatsCurve, TEXT("BaseStatsCurveTable"));
	// LoadDataTable(CharacterStaticDataTable, LoadedCharacterStaticData, TEXT("CharacterStaticData"));
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
	CacheDataTable<FCharacterStaticData, FName>(
		LoadedCharacterStaticData, CachedCharacterStaticData, &FCharacterStaticData::RowName, TEXT("CharacterStaticData"));

	//장비 정적데이터 캐싱
	CacheDataTable<FEquipmentStaticData, FName>(
		LoadedEquipmentStaticData, CachedEquipmentStaticData, &FEquipmentStaticData::RowName, TEXT("EquipmentStaticData"));
	//장비 스탯 보너스 데이터 캐싱
	CacheDataTable<FEquipmentBonus, FName>(
		LoadedEquipmentStatBonus, CachedEquipmentBonus, &FEquipmentBonus::RowName, TEXT("EquipmentBonus"));
	
	//세트 장비 효과 데이터 캐싱
	CacheDataTable<FSetEffectData, FName>(
		LoadedSetEffectBonus, CachedSetEffectData, &FSetEffectData::RowName, TEXT("SetEffectData"));
	
	//스킬 데이터 캐싱
	CacheDataTable<FSkillStaticData, FName>(
		LoadedSkillStaticData, CachedSkillStaticData, &FSkillStaticData::RowName, TEXT("SkillStaticData"));
	
	//에너미 데이터 캐싱
	CacheDataTable<FEnemyStaticData, FName>(
		LoadedEnemyStaticData, CachedEnemyStaticData, &FEnemyStaticData::RowName, TEXT("EnemyStaticData"));

	//스테이지 데이터 캐싱
	CacheDataTable<FStageStaticData, FName>(
		LoadedStageStaticData, CachedStageStaticData, &FStageStaticData::RowName, TEXT("StageStaticData"));
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

FName UGameDataSubsystem::SetTypeToName(ELRSetItemType SetType)
{
	switch (SetType)
	{
	case ELRSetItemType::BASIC : return FName(TEXT("BASIC"));
	case ELRSetItemType::FIRE : return FName(TEXT("FIRE"));
	case ELRSetItemType::ICE : return FName(TEXT("ICE"));
	case ELRSetItemType::ELECTRIC : return FName(TEXT("ELECTRIC")); 
		
	default:
		{
			LR_WARN(TEXT("Unknown SetType"));
			return FName(TEXT("BASIC"));
		}
	}
}

// ========================================
// 캐릭터 데이터 조회
// ========================================

const FCharacterStaticData& UGameDataSubsystem::GetCharacterStaticData(FName CharacterID) const
{
	return GetCachedData(CachedCharacterStaticData, CharacterID, EmptyCharacterStaticData, TEXT("CharacterStaticData"));
}


float UGameDataSubsystem::GetCharacterFinalStat(FName CharacterID, ELRStatusType StatusType, int32 CharacterLevel)
{
	float baseStat = GetBaseStatAtLevel(StatusType, CharacterLevel); //베이스 스탯
	float multiplier = GetStatusMultiplier(CharacterID, StatusType); //스탯 승수
	float finalStat = baseStat * multiplier; //최종 스탯
	
	LR_INFO(TEXT("CharacterID : %s, Stat : %s, Level : %d, Base = %.1f * Mult = %.2f = %.1f"), 
		*CharacterID.ToString(), *StatTypeToName(StatusType).ToString(), CharacterLevel, baseStat, multiplier, finalStat);
	
	return finalStat;
}


TArray<FName> UGameDataSubsystem::GetAllCharacterIDs()
{
	TArray<FName> characterIDs;
	CachedCharacterStaticData.GetKeys(characterIDs);
	
	LR_INFO(TEXT("Found %d Characters"), characterIDs.Num());
	
	return characterIDs;
}

// ========================================
// 장비 데이터 조회
// ========================================
const FEquipmentStaticData& UGameDataSubsystem::GetEquipmentStaticData(FName EquipmentID) const
{
	return GetCachedData(CachedEquipmentStaticData, EquipmentID, EmptyEquipmentStaticData, TEXT("EquipmentStaticData"));
}

const FEquipmentBonus& UGameDataSubsystem::GetEquipmentBonus(FName EquipmentID) const
{
	return GetCachedData(CachedEquipmentBonus, EquipmentID, EmptyEquipmentBonus, TEXT("EquipmentBonus"));
}

float UGameDataSubsystem::GetEquipmentStatBonus(FName EquipmentID, ELRStatusType StatusType, int32 EquipmentLevel)
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
			LR_WARN(TEXT("Invalid EnumType"));
			break;
		}
	}
	
	//공식 : 기본 보너스 + (레벨 - 1) * 레벨당 보너스
	return baseAmount + (EquipmentLevel - 1) * bonusPerLevel;
}

float UGameDataSubsystem::GetTotalEquipmentBonus(const TArray<FName>& EquipmentIDs,
	const TArray<int32>& EquipmentLevels, ELRStatusType StatusType)
{
	float totalBonus = 0.f;
	int32 count = FMath::Min(EquipmentIDs.Num(), EquipmentLevels.Num()); //유효한 갯수만.
	
	for (int i = 0; i < count; i++)
	{
		if (EquipmentIDs[i] != NAME_None)
		{
			totalBonus += GetEquipmentStatBonus(EquipmentIDs[i], StatusType, EquipmentLevels[i]);
		}
	}
	
	return totalBonus;
}

TArray<FName> UGameDataSubsystem::GetAllEquipmentIDs()
{
	TArray<FName> equipmentIDs;
	CachedEquipmentStaticData.GetKeys(equipmentIDs);
	
	LR_INFO(TEXT("Found %d Equipments"), equipmentIDs.Num());
	
	return equipmentIDs;
}


// ========================================
// 세트 장비 효과 데이터 조회
// ========================================
const FSetEffectData& UGameDataSubsystem::GetSetEffectData(FName SetID) const
{
	return GetCachedData(CachedSetEffectData, SetID, EmptySetEffectData, TEXT("SetEffectData"));
}


TArray<FName> UGameDataSubsystem::CheckActiveSetIDs(const TArray<FName>& EquipmentIDs) const
{
	//세트별 장착갯수 집계
	TMap<ELRSetItemType, int32> setCounts; 
	
	for (const FName& id : EquipmentIDs)
	{
		if (id == NAME_None)
		{
			continue;
		}
		
		FEquipmentStaticData data = GetEquipmentStaticData(id);
		
		if (data.SetType != ELRSetItemType::NONE) //기본 제외
		{
			setCounts.FindOrAdd(data.SetType)++;
		}
	}
	
	//활성화 조건 체크
	TArray<FName> activeSets;
	
	for (auto& [setType, count] : setCounts)
	{
		FName SetName = SetTypeToName(setType);
		//세트 달성여부 확인
		const FSetEffectData& setData = GetSetEffectData(SetName);
		
		if (count >= setData.RequiredPieces)
		{
			activeSets.Add(SetName);
			LR_INFO(TEXT("Set %s activated : %d/%d pieces"), *SetName.ToString(), count, setData.RequiredPieces);
		}
	}
	
	return activeSets;
}

void UGameDataSubsystem::GetSetEffectStatBonus(const TArray<FName>& EquipmentIDs, float& OutHPBonus, float& OutAtkBonus,
	float& OutDefBonus)
{
	OutHPBonus = 1.f;
	OutAtkBonus = 1.f;
	OutDefBonus = 1.f;
	
	//세트효과 활성화 체크
	TArray<FName> activeSets = CheckActiveSetIDs(EquipmentIDs);
	
	for (const FName& setID : activeSets)
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
		
		LR_INFO(TEXT("get bonus from set equipment(%s) : HP * %.0f, ATK * %0.f, DEF * %0.f"), 
			*setID.ToString(), data.BonusHPPercent, data.BonusAttackPercent, data.BonusDefensePercent);
	}
}



// ========================================
// 스킬 데이터 조회
// ========================================
const FSkillStaticData& UGameDataSubsystem::GetSkillStaticData(FName SkillID) const
{
	return GetCachedData(CachedSkillStaticData, SkillID, EmptySkillStaticData, TEXT("SkillStaticData"));
}


TArray<FName> UGameDataSubsystem::GetCharacterSkillIDs(FName CharacterID)
{
	FCharacterStaticData data = GetCharacterStaticData(CharacterID);
	
	return data.SkillIDs;
}

TArray<FName> UGameDataSubsystem::GetEnemySkillIDs(FName EnemyID)
{
	FEnemyStaticData data = GetEnemyStaticData(EnemyID);

	return data.SkillIDs;
}

TArray<FName> UGameDataSubsystem::GetEquipmentSkillIDs(FName EquipmentID)
{
	FEquipmentStaticData data = GetEquipmentStaticData(EquipmentID);
	
	return data.SkillIDs;
}

const FEnemyStaticData& UGameDataSubsystem::GetEnemyStaticData(FName EnemyID) const
{
	return GetCachedData(CachedEnemyStaticData, EnemyID, EmptyEnemyStaticData, TEXT("EnemyStaticData"));
}

TArray<FName> UGameDataSubsystem::GetAllEnemyIDs()
{
	TArray<FName> EnemyIDs;
	CachedEnemyStaticData.GetKeys(EnemyIDs);
	
	LR_INFO(TEXT("Found %d Enemy"), EnemyIDs.Num());
	
	return EnemyIDs;
}

const FStageStaticData& UGameDataSubsystem::GetStageStaticData(FName StageID) const
{
	return GetCachedData(CachedStageStaticData, StageID, EmptyStageStaticData, TEXT("StageStaticData"));
}