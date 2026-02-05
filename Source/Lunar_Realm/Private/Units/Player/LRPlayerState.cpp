// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/LRPlayerState.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"

ALRPlayerState::ALRPlayerState()
{
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); 
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	
	AttributeSet = CreateDefaultSubobject<ULRPlayerAttributeSet>(TEXT("AttributeSet"));

	
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ALRPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitializePlayerData();
}

UAbilitySystemComponent* ALRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRPlayerState::InitializePlayerData()
{
	// TODO: SaveGameSubsystem에서 플레이어 데이터 로드
	// 지금은 임시 데이터 넣음
	CharacterID = 10101;
	CharacterLevel = 1;

	EquippedItems.Add(EEquipmentType::WEAPON, 20100101);
	EquippedItemLevels.Add(EEquipmentType::WEAPON, 1);

	// 스텟 계산
	InitializeAttributes();

	// 스킬부여
	GrantCharacterAbilities();

	// 장비 스킬 부여
	for(auto& Elem: EquippedItems)
	{
		GrantEquipmentAbilities(Elem.Key, Elem.Value);
	}

	UE_LOG(LogTemp, Log, TEXT("[LRPlayerState] 플레이어의 데이터 모두 가져옴"));

}

void ALRPlayerState::InitializeAttributes()
{
	if (!AttributeSet)
	{
		return;
	}

	// 서브시스템 가져오기
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}
	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem || !AbilitySystemComponent)
	{
		return;
	}

	float CharHP = DataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::HP, CharacterLevel);
	float CharAtk = DataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::ATK, CharacterLevel);
	float CharDef = DataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::DEF, CharacterLevel);

	// 장비보너스 계산
	TArray<int32> ItemIDs;
	TArray<int32> ItemLevels;
	EquippedItems.GenerateValueArray(ItemIDs);
	EquippedItemLevels.GenerateValueArray(ItemLevels);

	float EquipHP = DataSubsystem->GetTotalEquipmentBonus(ItemIDs, ItemLevels, ELRStatusType::HP);
	float EquipAtk = DataSubsystem->GetTotalEquipmentBonus(ItemIDs, ItemLevels, ELRStatusType::ATK);
	float EquipDef = DataSubsystem->GetTotalEquipmentBonus(ItemIDs, ItemLevels, ELRStatusType::DEF);

	// 세트 효과
	float SetHP_Mul = 1.0f;
	float SetAtk_Mul = 1.0f;
	float SetDef_Mul = 1.0f;

	// 참조로 값 받아옴
	DataSubsystem->GetSetEffectStatBonus(ItemIDs, SetHP_Mul, SetAtk_Mul, SetDef_Mul);

	float FinalHP = (CharHP + EquipHP) * SetHP_Mul;
	float FinalAtk = (CharAtk + EquipAtk) * SetAtk_Mul;
	float FinalDef = (CharDef + EquipDef) * SetDef_Mul;

	AttributeSet->InitHealth(FinalHP);
	AttributeSet->InitMaxHealth(FinalHP);
	AttributeSet->InitAttackPower(FinalAtk);

	UE_LOG(LogTemp, Log, TEXT("Final Stats - HP: %.1f, ATK: %.1f, DEF: %.1f"), FinalHP, FinalAtk, FinalDef);

}

void ALRPlayerState::EquipItem(EEquipmentType Slot, int32 ItemID)
{
	UnequipItem(Slot);

	EquippedItems.Add(Slot, ItemID);
	EquippedItemLevels.Add(Slot, 1);

	GrantEquipmentAbilities(Slot, ItemID);

	InitializeAttributes();
	
}

void ALRPlayerState::UnequipItem(EEquipmentType Slot)
{
	// 기존 스킬 제거
	if (EquipmentAbilityHandles.Contains(Slot))
	{
		for (const FGameplayAbilitySpecHandle& Handle : EquipmentAbilityHandles[Slot])
		{
			AbilitySystemComponent->ClearAbility(Handle);
		}
		EquipmentAbilityHandles.Remove(Slot);
	}

	EquippedItems.Remove(Slot);
	EquippedItemLevels.Remove(Slot);

	InitializeAttributes();

}

void ALRPlayerState::GrantCharacterAbilities()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem || !AbilitySystemComponent) return;

	// 캐릭터 id로 스킬 id 목록 가져오기
	TArray<int32> SkillIDs = DataSubsystem->GetCharacterSkillIDs(CharacterID);

	for (int32 SkillID : SkillIDs)
	{
		// 스킬 id로 정적 데이터 가져오기
		const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(SkillID);

		for (TSubclassOf<UGameplayAbility> AbilityClass : SkillData.GrantedAbilities)
		{
			if (AbilityClass)
			{
				FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
				FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
				CharacterAbilityHandles.Add(Handle);
			}
		}
	}
}

void ALRPlayerState::GrantEquipmentAbilities(EEquipmentType Slot, int32 EquipmentID)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem || !AbilitySystemComponent) return;

	// 1. 장비 ID로 스킬 ID 목록 조회
	TArray<int32> SkillIDs = DataSubsystem->GetEquipmentSkillIDs(EquipmentID);

	TArray<FGameplayAbilitySpecHandle> NewHandles;

	for (int32 SkillID : SkillIDs)
	{
		const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(SkillID);

		for (TSubclassOf<UGameplayAbility> AbilityClass : SkillData.GrantedAbilities)
		{
			if (AbilityClass)
			{
				FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
				FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
				NewHandles.Add(Handle);
			}
		}
	}

	// 핸들 저장 (나중에 장비 해제할 때 제거용)
	EquipmentAbilityHandles.Add(Slot, NewHandles);
}
