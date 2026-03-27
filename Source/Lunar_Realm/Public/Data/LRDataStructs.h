// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Data/LREnumType.h"
#include "Engine/DataTable.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "MediaSource.h"
#include "LRDataStructs.generated.h"



// =============================================================================
/** 
 * FCharacterStaticData 구성 요소
 * - 캐릭터 정적 데이터(도감 데이터 사용)
 * - ID, 이름, 캐릭터 설명, 캐릭터 분류 필드, 이미지 텍스쳐
 * - 캐릭터 레벨별 스테이터스 승수 데이터(체력/공격력/방어력)
 * - 새로운 스탯 요소 필요시 Row 추가 가능
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경. 분류 필드 추가
// (260211) BJM 소환할 캐릭터 매쉬, 애님블루프린트 변수 추가
// (260212) BJM 맴버 캐릭터 공격, 사망 몽타주 추가
// (260212) KWB BehaviorTree 필드 추가
// (260213) BJM 소환관련 cost, cooltime, icon 항목 추가
// (260220) BJM GA항목 추가
// (260224) KHS 스킬효과 데이터 추가.
// (260225) BJM 플레이어가 직접 조종할 때 쓸 단일 평타 GA
// (260316) BJM 플레이어 Ingame 아이콘 테두리, GradeImage 추가
// (260322) KWB FEnemySoundData 추가
// =============================================================================

USTRUCT(BlueprintType)
struct FCharacterStaticData : public FTableRowBase
{
	GENERATED_BODY()
    
	//참조키
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName DataID; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FString CharacterName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText Description;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRGrade Grade; //SSR, SR, R, N
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRAttackType AttackType; //Melee, Ranged
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRClassType ClassType; //Warrior, Mage, etc
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Multiplier")
	float HPMultiplier;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Multiplier")
	float AttackMultiplier;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Multiplier")
	float DefenseMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> CharacterTexture;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<FName> SkillIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftClassPtr<UAnimInstance> AnimBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UAnimMontage> NormalAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Summon")
	float SummonCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Summon")
	float SummonCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	TSoftObjectPtr<UTexture2D> PortraitIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<TSubclassOf<class ULRGameplayAbilityBase>> MemberAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TSubclassOf<class ULRGameplayAbilityBase> PlayerBasicAttackAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	FVector PlayerScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	FVector MemberScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Equipment")
	FName MemberWeaponID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	TSoftObjectPtr<UTexture2D> PortraitFrame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	TSoftObjectPtr<UTexture2D> GradeImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	TSoftObjectPtr<UTexture2D> WholeBodyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> CutInImage;



};



// =============================================================================
/** 
 * FPlayerCharacterInstance 구성 요소
 * - 런타임중 사용하는 실제 인스턴스 공유 데이터
 * - SaveGame 로드 -> InventorySubsys -> 실제 런타임 인스턴스
 * - 아웃게임 UI(도감, 파티 편성), 인게임 PlayerState (레벨 정보) 등에서 사용
 * - 캐릭터 ID, 캐릭터레벨, 캐릭터 경험치, 캐릭터 해금상태
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경. 분류 필드 추가
// =============================================================================
USTRUCT(BlueprintType)
struct FCharacterInstance
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FName CharacterID;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentLevel;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentExp;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FDateTime AcquisitionTime; //획득 타임스탬프
	
	FCharacterInstance() 
		: CharacterID(NAME_None), CurrentLevel(1), CurrentExp(0), bIsUnlocked(false), AcquisitionTime(FDateTime::MinValue())
	{	}
    
	FCharacterInstance(FName InID, int32 InLevel = 1)
		: CharacterID(InID), CurrentLevel(InLevel), CurrentExp(0), bIsUnlocked(true), AcquisitionTime(FDateTime::Now())
	{}
};


// =============================================================================
/** 
 * FEquipmentStaticData 구성 요소
 * - 장비별 정적 데이터(도감용)
 * - 장비ID, 이름, 설명, 2D텍스쳐, 장비별 GameAbility(GAS)
 * - TSoftObjectPtr로 비동기 로딩.
 * - 연동된 게임 어빌리티 정보를 통해 장비스킬 가능하도록 연결(DataSubsys)
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경. 분류 필드 추가
// =============================================================================
USTRUCT(BlueprintType)
struct FEquipmentStaticData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName DataID; //EQUIP_FIRE_SWORD
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FString EquipmentName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Resource")
	TSoftObjectPtr<UStaticMesh> EquipmentMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRGrade Grade;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRItemType ItemType; //WEAPON, ARMOR.
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRSetItemType SetType;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> EquipmentTexture;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<FName> SkillIDs;


	
};


// =============================================================================
/** 
 * FEquipmentBonus 구성 요소
 * - 장비별 스탯 보너스 데이터
 * - 장비ID, 각 스테이터스별 보너스 스탯 정보, 레벨별 보너스 스탯 가산 정보.
 * - TSoftObjectPtr로 비동기 로딩.
 * - TODO 이후 CT형태로 변경 
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경.
// =============================================================================
USTRUCT(BlueprintType)
struct FEquipmentBonus : public FTableRowBase
{
	GENERATED_BODY()
    
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	FName DataID; //EquipmentStaticData의 RowName 참조 외래키
    
	// 레벨별 스탯 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float BaseHPBonus = 100.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float HPBonusPerLevel = 50.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float BaseAttackBonus = 20.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float AttackBonusPerLevel = 10.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float BaseDefenseBonus = 10.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	float DefenseBonusPerLevel = 5.f;
    
	// TODO 이후 CurveTable 사용
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stats")
	// TSoftObjectPtr<UCurveTable> StatBonusCurve;
};



// =============================================================================
/** 
 * FPlayerEquipmentInstance 구성 요소
 * - 런타임중 사용하는 실제 인스턴스 공유 데이터
 * - SaveGame 로드 -> CollectionSubsys -> 실제 런타임 인스턴스
 * - 아웃게임 UI(도감, 장비 장착), 인게임 PlayerState (장비 정보) 등에서 사용
 * - 장비 ID, 장비 레벨, 장비 해금상태
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경. 
// =============================================================================
USTRUCT(BlueprintType)
struct FEquipmentInstance
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FName EquipmentID;
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FGuid InstanceID; //개별 인스턴스 구분용
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentLevel;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 CurrentExp;
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 MaxExp; //TODO 이후 수정
	
	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked;
    
	UPROPERTY(SaveGame, BlueprintReadWrite)
	FDateTime AcquisitionTime;
	
	FEquipmentInstance() 
		: EquipmentID(NAME_None), InstanceID(FGuid::NewGuid()), CurrentLevel(1), CurrentExp(0), MaxExp(100), bIsUnlocked(false), AcquisitionTime(FDateTime::MinValue())
	{}
    
	FEquipmentInstance(FName InID, int32 InLevel = 1)
		: EquipmentID(InID), InstanceID(FGuid::NewGuid()), CurrentLevel(InLevel), CurrentExp(0), MaxExp(100), bIsUnlocked(true), AcquisitionTime(FDateTime::Now())
	{}
	
	
};

// =============================================================================
/** 
 * FSetEffectData 구성 요소
 * - 장비세트효과 데이터
 * - 세트ID, 세트 이름, 필요 장착갯수, 스탯 퍼센트 보너스
 */
//=============================================================================
// (260126) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경. 
// =============================================================================
USTRUCT(BlueprintType)
struct FSetEffectData : public FTableRowBase
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set")
	FName DataID;  // 02 = 화염, 03 = 얼음
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set")
	FString SetName;  // "Fire Set", "Ice Set"
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Set")
	int32 RequiredPieces = 3;  // 3개 풀세트
    
	// 스탯 퍼센트 보너스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BonusHPPercent = 0.f;  // 10.0 = 10% 증가
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BonusAttackPercent = 0.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BonusDefensePercent = 0.f;
};




// =============================================================================
/** 
 * FSkillStaticData 구성 요소
 * - 스킬 정적 데이터
 * - ID, 이름, 스킬 설명, 이미지 텍스쳐, 실제 사용 GA클래스
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
//=============================================================================
// (260126) KHS 제작. 제반 사항 구현.
// (260210) KHS Int타입 참조키를 FName타입으로 변경. 
// =============================================================================
USTRUCT(BlueprintType)
struct FSkillStaticData : public FTableRowBase
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillID; //SKILL_FIREBALL
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillEffectID; //DT_SkillEffect(FK)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResourceID; //DT_SkillResource FK
	
	// 실제 GA 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftClassPtr<UGameplayAbility>> GrantedAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillTag; //스킬 태그
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SkillName;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	
};

// (260226) KHS v1.2 신규 추가 — 스킬 리소스 데이터
USTRUCT(BlueprintType)
struct FSkillResourceData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResourceID; // RESOURCE_ARROW

	// UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> SkillIcon;

	// 스폰 시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UNiagaraSystem> SpawnVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> SpawnSFX;

	// 비행 중 트레일
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UNiagaraSystem> TrailVFX;

	// 충돌/소멸 시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UNiagaraSystem> ImpactVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> ImpactSFX;
};


// =============================================================================
/** 
 * FSkillEffectData  구성 요소
 * - GA에서 사용하기 위한 스킬 효과 정적 데이터
 */
//=============================================================================
// (260224) KHS 제작. 제반 사항 구현.
// (260226) KHS v1.2 개편. SkillType/BasicCount/Range 제거,
//          FlightType/HitType/Lifetime/ExpireCondition 추가
// =============================================================================
USTRUCT(BlueprintType)
struct FSkillEffectData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillEffectID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StatusEffectID; // DT_StatusEffect FK (기존 BuffEffectID → 이름 변경)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFlightType FlightType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHitType HitType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lifetime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount; //데미지 or 힐링 amount
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range; //스킬 사용 범위

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown; //스킬 쿨타임 GE 사용 간격.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EExpireCondition ExpireCondition;
};

// =============================================================================
// (260226) KHS v1.2 신규 추가 — 스폰 파라미터
// =============================================================================
USTRUCT(BlueprintType)
struct FSkillSpawnData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillSpawnID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillEffectID; // DT_SkillEffect FK

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ProjectileCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SocketName; // 유효하지 않으면 캐릭터 전방 2m에서 스폰

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESpawnPattern SpawnPattern;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpreadAngle = 0.f; // FanSpread 전용
};

// =============================================================================
// (260226) KHS v1.2 신규 추가 — 비행 타입별 전용 파라미터
// =============================================================================
USTRUCT(BlueprintType)
struct FFlightHomingData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName HomingID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillEffectID; // DT_SkillEffect FK

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TurnSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LockRange;
};

USTRUCT(BlueprintType)
struct FFlightArcData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ArcID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillEffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LaunchAngle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GravityScale;
};

USTRUCT(BlueprintType)
struct FFlightPierceData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PierceID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillEffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PierceCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DamageDecay; // 관통마다 데미지 감소율
};

USTRUCT(BlueprintType)
struct FFlightExplodeData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ExplodeID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillEffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ExplosionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ExplosionDamageMultiplier; // 거리 기반 감쇠 배율
};

// =============================================================================
// (260226) KHS v1.2 신규 추가 — 범위 타격 전용
// =============================================================================
USTRUCT(BlueprintType)
struct FSkillHitAreaData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName HitAreaID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillEffectID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HitRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxTargetCount; // 0 = 제한 없음
};

// =============================================================================
/** 
 * FSkillObjectInitData 구성 요소
 * - GA->소환 오브젝트로 전달하는 초기값 데이터
 */
//=============================================================================
// (260224) KHS 제작. 제반 사항 구현.
// =============================================================================
USTRUCT(BlueprintType)
struct FSkillObjectInitData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass; // 데미지 GE
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> StatusEffectClass; // 상태이상 GE (없으면 nullptr)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UAbilitySystemComponent> InstigatorASC; // 발사자 ASC

	//공통 프로퍼티
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lifetime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSkillSpawnData SpawnData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillEffectID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResourceID; 
	
};
//
// // Pierce 전용 확장
// USTRUCT(BlueprintType)
// struct FPierceSkillObjectInitData : public FSkillObjectInitData
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	int32 PierceCount;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float DamageDecay;
// };
//
// // Explode/Arc 전용 확장
// USTRUCT(BlueprintType)
// struct FExplodeSkillObjectInitData : public FSkillObjectInitData
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float ExplosionRadius;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float ExplosionDamageMultiplier;
// };
//
// // Homing 전용 확장
// USTRUCT(BlueprintType)
// struct FHomingSkillObjectInitData : public FSkillObjectInitData
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float TurnSpeed;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float LockRange;
// };
//
// // Arc 전용 확장
// USTRUCT(BlueprintType)
// struct FArcSkillObjectInitData : public FSkillObjectInitData
// {
// 	GENERATED_BODY()
// 	
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float ExplosionRadius;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float ExplosionDamageMultiplier;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float LaunchAngle;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	float GravityScale;
// };


// =============================================================================
/** 
 * FStatusEffectData 구성 요소
 * - 스킬로 인한 버프/디버프 효과 정적데이터
 */
//=============================================================================
// (260224) KHS 제작. 제반 사항 구현.
// (260226) KHS v1.2 신규 추가 — 상태이상 정의 (기존 FBuffEffectData 대체)
// =============================================================================
USTRUCT(BlueprintType)
struct FStatusEffectData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StatusEffectID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusType StatusType; // 버프/디버프 타입 문자열 → ParseBuffType()으로 변환
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag GameplayTag; // GAS 상태이상 태그
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UGameplayEffect> StatusEffectGE; // 상태이상 GE 에셋 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StackLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName StackPolicy;
};



// =============================================================================
/**
 * FEnemyStaticData 구성 요소
 * - 캐릭터 정적 데이터(인게임 스폰 데이터)
 * - ID, 이름, 적 설명(TBD), 이동 속도, 공격력, 체력, 이미지 텍스쳐(TBD - 보스), 실제 사용 GA클래스(TBD - 노말, 엘리트 - 1개 /보스 - 3개)
 * - 이미지의 경우 SoftObjectPtr로 비동기 로딩
 */
 //=============================================================================
 // (260204) KWB 제작. 제반 사항 구현.
 // (260209) KWB 멤버 추가 및 순서 변경, 헤더 추가("Engine/SkeletalMesh.h")
 // (260210) KHS Int타입 참조키를 FName타입으로 변경. 분류 필드 추가
 // (260212) KWB BehaviorTree 필드 추가
 // =============================================================================
USTRUCT(BlueprintType)
struct FEnemyStaticData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName DataID; //ENEMY_GOBLIN_NORMAL

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	int32 DropAether;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRGrade Grade;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELRAttackType AttackType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Classification")
	ELREnemyType EnemyType; //GOBLIN, GOLEM...

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spec")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<USkeletalMesh> EnemyMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UTexture2D> CharacterTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftClassPtr<UAnimInstance> AnimBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TArray<TSoftObjectPtr<UAnimMontage>> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UAnimMontage> AttackedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TSoftObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Visual")
	TArray<TSoftObjectPtr<UNiagaraSystem>> AuraVFXList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skills")
	TArray<FName> SkillIDs;
};

// =============================================================================
/**
 * FStageStaticData 구성 요소
 * - 스테이지 데이터 드리븐 전투를 위한 정적 데이터
 * - StageID 기준으로 해당 스테이지 소환 후보 EnemyID/가중치, 스폰 주기, 보상 정보 등을 제공
 *
 * NOTE(260208, Codex):
 * - 임시 데이터 필드 수정 필요
 * - StageID 관리 주체는 GameInstance로 가정, Spawner는 해당 ID로 데이터 조회함.
 */
 //=============================================================================
 // (260208) KWB 제작. 제반 사항 구현.
 // (260210) KHS Int타입 참조키를 FName타입으로 변경. 
 // (260318) KWB BossEnemyID 멤버 추가
 // (260319) BJM PlayerStartTag 멤버 추가 (스테이지별 플레이어 시작 위치 지정용)
 // =============================================================================
USTRUCT(BlueprintType)
struct FStageStaticData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName DataID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText StageName;

	// 스테이지에서 소환 가능한 Enemy ID 리스트 (1~4종 가정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TArray<FName> SpawnEnemyIDs;

	// SpawnEnemyIDs와 같은 인덱스의 확률 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TArray<float> SpawnWeights;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	float SpawnInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Boss")
	bool bIsBossStage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Boss")
	FName BossEnemyID = NAME_None;

	// ===== 확장/메타 필드 (UI/보상 연동용) =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 RewardGold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 RewardNormalTicket = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 RewardEnhanceTicket = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Reward")
	int32 RewardExp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stage")
	FName PlayerStartTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Stage")
	FName NextStageID;

};


 //=============================================================================
 // (260312) PJB 제작.
 // =============================================================================

USTRUCT(BlueprintType)
struct FChapterStaticData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName DataID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FText ChapterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TArray<FName> StageDataIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TSoftObjectPtr<UTexture2D> ChapterThumbnail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TSoftObjectPtr<UTexture2D> ChapterBackground;

};


USTRUCT(BlueprintType)
struct FCurrencyStaticData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName DataID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	ELRCurrencyType CurrencyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Spawn")
	TSoftObjectPtr<UTexture2D> CurrencyImage;

};


// =============================================================================
// (260210) PYI 제작
// (260312) PYI 가챠 리빌 연출 전용 데이터 추가
// (260320) PYI 리빌 화면 오픈 시 재생할 사운드 추가
// =============================================================================
// Gacha Data Structs (Banner/Pool/Rate/DuplicateReward/Result/Txn)
// =============================================================================
// Gacha Reveal Visual Data
// - 가챠 리빌 연출 전용 데이터
// - 게임 정적 데이터(Character/Equipment StaticData)와 분리하여 관리
// - 배경 / 리빌 전용 이미지 / 영상 확장 포인트 포함
// =============================================================================

// 배너(뽑기) 설정 DataTable
USTRUCT(BlueprintType)
struct FLRGachaBannerRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BannerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRCurrencyType CostCurrencyType = ELRCurrencyType::CrescentTicket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostSingle = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CostTen = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsePity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PityThreshold = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity PityGuaranteedRarity = ELRGachaRarity::UR;
};

// 배너 풀 DT
USTRUCT(BlueprintType)
struct FLRGachaPoolRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BannerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity Rarity = ELRGachaRarity::N;
};

// 중복 보상(등급별 골드 전환량) DT
USTRUCT(BlueprintType)
struct FLRGachaDuplicateRewardRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity Rarity = ELRGachaRarity::N;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GoldAmount = 10;
};

// 1회 결과(연출/UI에 넘길 데이터)
USTRUCT(BlueprintType)
struct FLRGachaResult
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRGachaRarity Rarity = ELRGachaRarity::N;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	bool bIsNew = false;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	bool bConvertedToGold = false;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 ConvertedGoldAmount = 0;
};

// 등급별 확률 DT
USTRUCT(BlueprintType)
struct FLRGachaRarityRateRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BannerID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaRarity Rarity = ELRGachaRarity::N;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rate = 0.0f;
};

// SaveGame에 저장될 Pending 트랜잭션
USTRUCT(BlueprintType)
struct FLRGachaPendingTransaction
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FGuid TxnId;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FName BannerID;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 DrawCount = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRCurrencyType CostCurrencyType = ELRCurrencyType::CrescentTicket;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 CostAmount = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 PrevPity = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 NewPity = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FLRGachaResult> Results;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ELRGachaTxnState State = ELRGachaTxnState::None;
};

// 리빌 연출용 DataTable Row
USTRUCT(BlueprintType)
struct FLRGachaRevealVisualRow : public FTableRowBase
{
	GENERATED_BODY()

	// 리빌 화면 오픈 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> RevealSFX;

	// 실루엣 -> 컬러 전환 순간 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundBase> ColorRevealSFX;

	// 실제 가챠 결과 ItemID와 동일하게 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	// 리빌 화면 배경
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> BackgroundTexture;

	// 리빌 메인 이미지
	// 비어 있으면 Character/Equipment StaticData의 기본 이미지 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> RevealTexture;

	// 최종 결과창 슬롯에 표시할 이미지
	// 비어 있으면 기존 StaticData 이미지로 fallback
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> ResultSlotTexture;

	// 나중에 영상 리빌 사용할 경우 확장용
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseVideo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMediaSource> RevealVideoSource;
};

// UI/리빌 위젯으로 넘기는 런타임 데이터
USTRUCT(BlueprintType)
struct FLRGachaRevealPresentationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName ItemID = NAME_None;

	UPROPERTY(BlueprintReadOnly)
	ELRGachaItemType ItemType = ELRGachaItemType::Hero;

	UPROPERTY(BlueprintReadOnly)
	ELRGachaRarity Rarity = ELRGachaRarity::N;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> BackgroundTexture = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> MainTexture = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMediaSource> VideoSource = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bUseVideo = false;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoundBase> RevealSound = nullptr;

	// 실루엣 -> 컬러 전환 시점 사운드
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoundBase> ColorRevealSound = nullptr;
};

// =============================================================================
/** * FCharacterSoundData 구성 요소
 * - 캐릭터 전용 사운드 데이터 (목소리 및 고유 효과음)
 * - 무기/스킬 타격음은 SkillResourceData에서 관리하므로 제외
 * - 사운드 에셋은 TSoftObjectPtr로 비동기 로딩하여 메모리 최적화
 * - 자주 반복되는 사운드(기합, 피격음 등)는 배열(TArray)로 구성하여 랜덤 재생 지원
 */
 // =============================================================================

//=============================================================================
// (260318) BJM 제작.
// =============================================================================

USTRUCT(BlueprintType)
struct FCharacterSoundData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName CharacterName;

	// ========================================
	// 캐릭터 목소리 대사
	// ========================================

	// 등장 / 소환 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> IntroVoice; 

	// 기본공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TArray<TSoftObjectPtr<USoundBase>> AttackGrunts;

	// 스킬 1 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> Skill1Voice; 

	// 스킬 2 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> Skill2Voice; 

	// 회복 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> HealVoice; 

	// 피격음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TArray<TSoftObjectPtr<USoundBase>> HitVoices;

	// 사망
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> DeathVoice; 

	// 체력 30% 이하 위기 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> LowHPVoice; 

	// 승리 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> VictoryVoice; 

	// 패배 대사
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> DefeatVoice;


	// ========================================
	// 캐릭터 행동 효과음
	// ========================================

	// 발소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Foley")
	TArray<TSoftObjectPtr<USoundBase>> FootstepSounds;

	// 사망시 효과음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Foley")
	TSoftObjectPtr<USoundBase> BodyFallSound;

	// 소환 효과음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Foley")
	TSoftObjectPtr<USoundBase> SummonSound;

};

//=============================================================================
// (260319) PJB 제작.
// =============================================================================
USTRUCT(BlueprintType)
struct FStageClearedData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadWrite)
	FName StageID;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	int32 StarMasking;

	UPROPERTY(SaveGame, BlueprintReadWrite)
	bool bIsUnlocked;

	FStageClearedData()
		: StageID(NAME_None), StarMasking(0), bIsUnlocked(false)
	{
	}

	FStageClearedData(FName InID)
		: StageID(InID), StarMasking(0), bIsUnlocked(false)
	{
	}
};

// =============================================================================
/** * FEnemySoundData 구성 요소
 * - 에너미 전용 사운드 데이터 (소리 및 고유 효과음)
 * - 공격 사운드(배열), 피격 사운드, 발걸음 사운드, 사망 사운드
 */
 // =============================================================================

//=============================================================================
// (260322) KWB 제작.
// (260325) KWB "개별 스킬 사운드 => 사운드 배열" 구조 변경
// =============================================================================

USTRUCT(BlueprintType)
struct FEnemySoundData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Basic")
	FName EnemyID;

	// 공격 사운드 (인덱스 = AttackMontages / SkillIDs와 1:1 대응)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Skill")
	TArray<TSoftObjectPtr<USoundBase>> AttackSounds;

	// 피격음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Effect")
	TArray<TSoftObjectPtr<USoundBase>> HitSound;

	// 발소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Effect")
	TArray<TSoftObjectPtr<USoundBase>> FootstepSounds;

	// 사망
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Effect")
	TSoftObjectPtr<USoundBase> DeathSound;

	// 등장 대사 (보스만 존재 예정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Voice")
	TSoftObjectPtr<USoundBase> IntroVoice;
};


USTRUCT(BlueprintType)
struct FSelectedInfo
{
	GENERATED_BODY()

	ECollectionType Type = ECollectionType::NONE;
	FName ID = NAME_None;
	int32 SlotIndex = -1;
	EPartyTaskType Task = EPartyTaskType::NONE;

	FSelectedInfo() = default;
	FSelectedInfo(ECollectionType InType, FName InID, int32 InSlotIndex = -1, EPartyTaskType InTask = EPartyTaskType::NONE)
		: Type(InType), ID(InID), SlotIndex(InSlotIndex), Task(InTask)
	{
	}
};