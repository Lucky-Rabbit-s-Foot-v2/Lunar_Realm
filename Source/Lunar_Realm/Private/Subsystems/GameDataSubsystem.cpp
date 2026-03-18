// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameDataSubsystem.h"
#include "Data/LRGameDataConfig.h"

//Static 기본값 정의(조회 실패시 반환용도)
FCharacterStaticData	UGameDataSubsystem::EmptyCharacterStaticData;
FCharacterSoundData		UGameDataSubsystem::EmptyCharacterSoundData;
FEquipmentStaticData	UGameDataSubsystem::EmptyEquipmentStaticData;
FEquipmentBonus			UGameDataSubsystem::EmptyEquipmentBonus;
FSetEffectData			UGameDataSubsystem::EmptySetEffectData;
FSkillStaticData		UGameDataSubsystem::EmptySkillStaticData;
FSkillResourceData		UGameDataSubsystem::EmptySkillResourceData;
FSkillEffectData		UGameDataSubsystem::EmptySkillEffectData;
FSkillSpawnData			UGameDataSubsystem::EmptySkillSpawnData;
FFlightHomingData		UGameDataSubsystem::EmptyFlightHomingData;
FFlightArcData			UGameDataSubsystem::EmptyFlightArcData;
FFlightPierceData		UGameDataSubsystem::EmptyFlightPierceData;
FFlightExplodeData		UGameDataSubsystem::EmptyFlightExplodeData;
FSkillHitAreaData		UGameDataSubsystem::EmptySkillHitAreaData;
FStatusEffectData		UGameDataSubsystem::EmptyStatusEffectData;
FEnemyStaticData		UGameDataSubsystem::EmptyEnemyStaticData;
FStageStaticData		UGameDataSubsystem::EmptyStageStaticData;
FChapterStaticData		UGameDataSubsystem::EmptyChapterStaticData;
FCurrencyStaticData		UGameDataSubsystem::EmptyCurrencyStaticData;


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
	CachedCharacterSoundData.Empty();
	CachedEquipmentStaticData.Empty();
	CachedEquipmentBonus.Empty();
	CachedSetEffectData.Empty();
	CachedSkillStaticData.Empty();
	CachedSkillResourceData.Empty();
	CachedSkillEffectData.Empty();  
	CachedSkillSpawnData.Empty();
	CachedFlightHomingData.Empty();
	CachedFlightArcData.Empty();
	CachedFlightPierceData.Empty();
	CachedFlightExplodeData.Empty();
	CachedSkillHitAreaData.Empty();
    CachedBuffEffectData.Empty();   
	CachedEnemyStaticData.Empty();
	CachedStageStaticData.Empty();
	CachedChapterStaticData.Empty();
	CachedCurrencyStaticData.Empty();

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
	LoadedBaseStatsCurve		 = Config->BaseStatsCurveTable.LoadSynchronous();		//베이스 스탯 커브
	LoadedCharacterStaticData	 = Config->CharacterStaticDataTable.LoadSynchronous();	//캐릭터 데이터
	LoadedCharacterSoundData	 = Config->CharacterSoundDataTable.LoadSynchronous();	//캐릭터 사운드 데이터
	LoadedEnemyStaticData		 = Config->EnemyStaticDataTable.LoadSynchronous();		//적 데이터
	LoadedEquipmentStaticData	 = Config->EquipmentStaticDataTable.LoadSynchronous();	//장비 데이터
	LoadedEquipmentStatBonus	 = Config->EquipmentStatBonusTable.LoadSynchronous();	//장비 보너스
	LoadedSetEffectBonus		 = Config->EquipmentSetEffectTable.LoadSynchronous();	//세트장비 효과
	LoadedSkillStaticData		 = Config->SkillStaticDataTable.LoadSynchronous();		//스킬 정적 데이터
	LoadedSkillResourceData		 = Config->SkillResourceDataTable.LoadSynchronous();	//스킬 리소스 데이터
	LoadedSkillEffectData		 = Config->SkillEffectDataTable.LoadSynchronous();		//스킬 GA 사용데이터
	LoadedSkillSpawnData		 = Config->SkillSpawnDataTable.LoadSynchronous();		//스킬 오브젝트 스폰 데이터
	LoadedSkillHitAreaData		 = Config->HitAreaDataTable.LoadSynchronous();			//스킬 오브젝트 효과범위 데이터
	LoadedStatusEffectData		 = Config->StatusEffectDataTable.LoadSynchronous();		//스킬 효과 버프/디버프 데이터
	LoadedSkillFlightHomingData  = Config->FlightHomingDataTable.LoadSynchronous();		//유도형 스킬 오브젝트 데이터
	LoadedSkillFlightArcData	 = Config->FlightArcDataTable.LoadSynchronous();		//궤도형 스킬 오브젝트 데이터
	LoadedSkillFlightPierceData  = Config->FlightPierceDataTable.LoadSynchronous();		//관통형 스킬 오브젝트 데이터
	LoadedSkillFlightExplodeData = Config->FlightExplodeDataTable.LoadSynchronous();	//폭발형 스킬 오브젝트 데이터
	LoadedStageStaticData		 = Config->StageStaticDataTable.LoadSynchronous();		//스테이지 데이터
	LoadedChapterStaticData		 = Config->ChapterStaticDataTable.LoadSynchronous();	//챕터 데이터
	LoadedCurrencyStaticData	 = Config->CurrencyStaticDataTable.LoadSynchronous();	//재화 데이터
	
	if (LoadedCharacterStaticData)
	{
		LR_INFO(TEXT("Character Data loaded with %d rows"), LoadedCharacterStaticData->GetRowNames().Num());
	}
	else
	{
		LR_ERROR(TEXT("CharacterStaticData is NULL"));
	}
	
}

void UGameDataSubsystem::CacheAllData()
{
	//ID기반으로 로드된 정보를 캐싱 데이터에 저장.
	//캐릭터 정적데이터 캐싱 
	CacheDataTable<FCharacterStaticData, FName>(
		LoadedCharacterStaticData, CachedCharacterStaticData, &FCharacterStaticData::DataID, TEXT("CharacterStaticData"));
	//캐릭터 사운드 데이터 캐싱
	CacheDataTable<FCharacterSoundData, FName>(
		LoadedCharacterSoundData, CachedCharacterSoundData, &FCharacterSoundData::CharacterName, TEXT("CharacterSoundData"));
	//장비 정적데이터 캐싱
	CacheDataTable<FEquipmentStaticData, FName>(
		LoadedEquipmentStaticData, CachedEquipmentStaticData, &FEquipmentStaticData::DataID, TEXT("EquipmentStaticData"));
	//장비 스탯 보너스 데이터 캐싱
	CacheDataTable<FEquipmentBonus, FName>(
		LoadedEquipmentStatBonus, CachedEquipmentBonus, &FEquipmentBonus::DataID, TEXT("EquipmentBonus"));
	//세트 장비 효과 데이터 캐싱
	CacheDataTable<FSetEffectData, FName>(
		LoadedSetEffectBonus, CachedSetEffectData, &FSetEffectData::DataID, TEXT("SetEffectData"));
	//스킬 데이터 캐싱
	CacheDataTable<FSkillStaticData, FName>(
		LoadedSkillStaticData, CachedSkillStaticData, &FSkillStaticData::SkillID, TEXT("SkillStaticData"));
	//GA 클래스 사용 데이터 캐싱
	CacheDataTable<FSkillEffectData, FName>(
		LoadedSkillEffectData, CachedSkillEffectData, &FSkillEffectData::SkillEffectID, TEXT("SkillEffectData"));
	// (260226) KHS v1.2 신규 추가
	CacheDataTable<FSkillResourceData, FName>(
	LoadedSkillResourceData, CachedSkillResourceData, &FSkillResourceData::ResourceID, TEXT("SkillResourceData"));
	CacheDataTable<FSkillSpawnData, FName>(
		LoadedSkillSpawnData, CachedSkillSpawnData, &FSkillSpawnData::SkillEffectID, TEXT("SkillSpawnData"));
	CacheDataTable<FFlightHomingData, FName>(
		LoadedSkillFlightHomingData, CachedFlightHomingData, &FFlightHomingData::SkillEffectID, TEXT("FlightHomingData"));
	CacheDataTable<FFlightArcData, FName>(
		LoadedSkillFlightArcData, CachedFlightArcData, &FFlightArcData::SkillEffectID, TEXT("FlightArcData"));
	CacheDataTable<FFlightPierceData, FName>(
		LoadedSkillFlightPierceData, CachedFlightPierceData, &FFlightPierceData::SkillEffectID, TEXT("FlightPierceData"));
	CacheDataTable<FFlightExplodeData, FName>(
		LoadedSkillFlightExplodeData, CachedFlightExplodeData, &FFlightExplodeData::SkillEffectID, TEXT("FlightExplodeData"));
	CacheDataTable<FSkillHitAreaData, FName>(
		LoadedSkillHitAreaData, CachedSkillHitAreaData, &FSkillHitAreaData::SkillEffectID, TEXT("HitAreaData"));
	CacheDataTable<FStatusEffectData, FName>(
		LoadedStatusEffectData, CachedBuffEffectData, &FStatusEffectData::StatusEffectID, TEXT("BuffEffectData"));
	//에너미 데이터 캐싱
	CacheDataTable<FEnemyStaticData, FName>(
		LoadedEnemyStaticData, CachedEnemyStaticData, &FEnemyStaticData::DataID, TEXT("EnemyStaticData"));
	//스테이지 데이터 캐싱
	CacheDataTable<FStageStaticData, FName>(
		LoadedStageStaticData, CachedStageStaticData, &FStageStaticData::DataID, TEXT("StageStaticData"));
	CacheDataTable<FChapterStaticData, FName>(
		LoadedChapterStaticData, CachedChapterStaticData, &FChapterStaticData::DataID, TEXT("ChapterStaticData"));
	CacheDataTable<FCurrencyStaticData, FName>(
		LoadedCurrencyStaticData, CachedCurrencyStaticData, &FCurrencyStaticData::DataID, TEXT("CurrencyStaticData"));

}

FName UGameDataSubsystem::StatTypeToName(ELRStatusType StatusType)
{
	switch (StatusType)
	{
	case ELRStatusType::HP : return FName(TEXT("HP")); 
	case ELRStatusType::ATK: return FName(TEXT("ATK"));
	case ELRStatusType::DEF : return FName(TEXT("DEF"));
	
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

EFlightType UGameDataSubsystem::ParseSkillType(FName TypeName)
{
	static const TMap<FName, EFlightType> SkillTypeToNameMap = 
	{
		{"Linear", EFlightType::LINEAR},
		{"Homing", EFlightType::HOMING},
		{"Arc", EFlightType::ARC},
		{"Pierce", EFlightType::PIERCE},
		{"Explode", EFlightType::EXPLODE}
	};
	
	const EFlightType* found = SkillTypeToNameMap.Find(TypeName);
	if (!found)
	{
		LR_WARN(TEXT("Unknown SkillType! : %s"), *TypeName.ToString());
		return EFlightType::LINEAR;
	}
	
	return *found;
}


EStatusType UGameDataSubsystem::ParseBuffType(FName TypeName)
{
	static const TMap<FName, EStatusType> BuffTypeToNameMap =
	{
		{"BUFF", EStatusType::BUFF},
		{"DEBUFF", EStatusType::DEBUFF}
	};
	
	const EStatusType* found = BuffTypeToNameMap.Find(TypeName);
	if (!found)
	{
		LR_WARN(TEXT("Unknown BuffType! : %s"), *TypeName.ToString());
		return EStatusType::BUFF;
	}
	
	return *found;
}

// ========================================
// 캐릭터 데이터 조회
// ========================================

const FCharacterStaticData& UGameDataSubsystem::GetCharacterStaticData(FName CharacterID) const
{
	return GetCachedData(CachedCharacterStaticData, CharacterID, EmptyCharacterStaticData, TEXT("CharacterStaticData"));
}

// (260318) BJM 캐릭터 사운드 데이터 조회 함수 추가
const FCharacterSoundData& UGameDataSubsystem::GetCharacterSoundData(FName CharacterName) const
{
	return GetCachedData(CachedCharacterSoundData, CharacterName, EmptyCharacterSoundData, TEXT("CharacterSoundData"));
}


float UGameDataSubsystem::GetCharacterFinalStat(FName CharacterID, ELRStatusType StatusType, int32 CharacterLevel)
{
	float baseStat = GetBaseStatAtLevel(StatusType, CharacterLevel); //베이스 스탯
	float multiplier = GetStatusMultiplier(CharacterID, StatusType); //스탯 승수
	float finalStat = baseStat * multiplier; //최종 스탯
	
//	LR_INFO(TEXT("CharacterID : %s, Stat : %s, Level : %d, Base = %.1f * Mult = %.2f = %.1f"), 
//		*CharacterID.ToString(), *StatTypeToName(StatusType).ToString(), CharacterLevel, baseStat, multiplier, finalStat);
	
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
	
	//TODO FName으로 변경후 세트 효과 체크가 제대로 되고있는지 디버깅 찍어봐야함.
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

// ========================================
// GA 데이터 조회
// ========================================

const FSkillEffectData& UGameDataSubsystem::GetSkillEffectData(FName SkillEffectID) const
{
	return GetCachedData(CachedSkillEffectData, SkillEffectID, EmptySkillEffectData, TEXT("SkillEffectData"));
}

const FSkillResourceData& UGameDataSubsystem::GetSkillResourceData(FName ResourceID) const
{
	return GetCachedData(CachedSkillResourceData, ResourceID, EmptySkillResourceData, TEXT("SkillResourceData"));
}

const FSkillSpawnData& UGameDataSubsystem::GetSkillSpawnData(FName SkillEffectID) const
{
	return GetCachedData(CachedSkillSpawnData, SkillEffectID, EmptySkillSpawnData, TEXT("SkillSpawnData"));
}


const FStatusEffectData& UGameDataSubsystem::GetStatusEffectData(FName StatusEffectID) const
{
	return GetCachedData(CachedBuffEffectData, StatusEffectID, EmptyStatusEffectData, TEXT("BuffEffectData"));
}

const FSkillHitAreaData& UGameDataSubsystem::GetSkillHitAreaData(FName SkillEffectID) const
{
	return GetCachedData(CachedSkillHitAreaData, SkillEffectID, EmptySkillHitAreaData, TEXT("SkillHitAreaData"));
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

const FChapterStaticData& UGameDataSubsystem::GetChapterStaticData(FName ChapterID) const
{
	return GetCachedData(CachedChapterStaticData, ChapterID, EmptyChapterStaticData, TEXT("ChapterStaticData"));
}

const TArray<FName> UGameDataSubsystem::GetAllStageIDsByChapterID(FName ChapterID) const
{
	const FChapterStaticData& ChapterData = GetChapterStaticData(ChapterID);
	return ChapterData.StageDataIDs;
}

const FCurrencyStaticData& UGameDataSubsystem::GetCurrencyStaticData(FName CurrencyID) const
{
	return GetCachedData(CachedCurrencyStaticData, CurrencyID, EmptyCurrencyStaticData, TEXT("CurrencyStaticData"));
}
