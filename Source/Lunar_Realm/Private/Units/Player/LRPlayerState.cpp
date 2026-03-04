// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/LRPlayerState.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Pawn.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "Core/Stage/LRStageGameState.h"
#include "Components/SkeletalMeshComponent.h"

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

	//InitializePlayerData();

	// (260303) KWB 델리게이트 바인딩을 위한 GameState 가져오기
	if (ALRStageGameState* GameState = GetWorld()->GetGameState<ALRStageGameState>())
	{
		GameState->OnAetherChanged.BindUObject(this, &ALRPlayerState::OnAetherReceived);
	}
	else
	{
		LR_WARN(TEXT("[PlayerState] GameState를 찾을 수 없어 델리게이트 바인딩 실패"));
	}
}

UAbilitySystemComponent* ALRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRPlayerState::InitializePlayerData()
{
	if (bIsPlayerDataInitialized)
	{
		return;
	}

	bIsPlayerDataInitialized = true;

	// TODO_BJM: SaveGameSubsystem에서 플레이어 데이터 로드
	// 지금은 임시 데이터 넣음
	CharacterID = FName("Nurse");
	CharacterLevel = FName("1");

	EquippedItems.Add(EEquipmentSlotType::WEAPON, FName(TEXT("EQUIP_MELEE_01")));
	EquippedItemLevels.Add(EEquipmentSlotType::WEAPON, 1);
	
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

	if (ALRPlayerCharacter* PC = Cast<ALRPlayerCharacter>(GetPawn()))
	{
		UGameInstance* GI = GetGameInstance();
		UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
		const FCharacterStaticData& CharData = DataSubsystem->GetCharacterStaticData(CharacterID);

		PC->GetMesh()->SetRelativeScale3D(CharData.PlayerScale);

		if (EquippedItems.Contains(EEquipmentSlotType::WEAPON))
		{
			PC->UpdateWeaponMesh(EquippedItems[EEquipmentSlotType::WEAPON]);
		}
	}

	//if (EquippedItems.Contains(EEquipmentSlotType::WEAPON))
	//{
	//	if (ALRPlayerCharacter* PC = Cast<ALRPlayerCharacter>(GetPawn()))
	//	{
	//		PC->UpdateWeaponMesh(EquippedItems[EEquipmentSlotType::WEAPON]);
	//	}
	//}
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

	int32 CharLevelInt = FCString::Atoi(*CharacterLevel.ToString());

	float CharHP = DataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::HP, CharLevelInt);
	float CharAtk = DataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::ATK, CharLevelInt);
	float CharDef = DataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::DEF, CharLevelInt);

	// 장비보너스 계산
	TArray<FName> ItemIDs;
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

	// 테스트용
	//AttributeSet->SetHealth(10.0f);
	//AttributeSet->InitAttackPower(15.0f);
	

	UE_LOG(LogTemp, Log, TEXT("Final Stats - HP: %.1f, ATK: %.1f, DEF: %.1f"), FinalHP, FinalAtk, FinalDef);

}

void ALRPlayerState::EquipItem(EEquipmentSlotType Slot, FName ItemID)
{
	UnequipItem(Slot);

	EquippedItems.Add(Slot, ItemID);
	EquippedItemLevels.Add(Slot, 1);

	GrantEquipmentAbilities(Slot, ItemID);

	InitializeAttributes();
	
	if (Slot == EEquipmentSlotType::WEAPON)
	{
		if (ALRPlayerCharacter* PC = Cast<ALRPlayerCharacter>(GetPawn()))
		{
			PC->UpdateWeaponMesh(ItemID);
		}
	}
}

void ALRPlayerState::UnequipItem(EEquipmentSlotType Slot)
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

	const FCharacterStaticData& CharData = DataSubsystem->GetCharacterStaticData(CharacterID);
	// 기본공격 GA 부여
	if (CharData.PlayerBasicAttackAbility)
	{
		FGameplayAbilitySpec Spec(CharData.PlayerBasicAttackAbility, 1, INDEX_NONE, this);
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
		CharacterAbilityHandles.Add(Handle);

		LR_WARN(TEXT("[PlayerState] 플레이어 평타 자동 장착 완료: %s"), *CharData.PlayerBasicAttackAbility->GetName());
	}

	// 캐릭터 id로 스킬 id 목록 가져오기
	TArray<FName> SkillIDs = DataSubsystem->GetCharacterSkillIDs(CharacterID);

	for (FName SkillID : SkillIDs)
	{
		// 스킬 id로 정적 데이터 가져오기
		const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(SkillID);

		for (const TSoftClassPtr<UGameplayAbility>& SoftAbilityClass : SkillData.GrantedAbilities)
		{
			if (SoftAbilityClass.IsNull())
			{
				LR_WARN(TEXT("Invalid Soft Class Reference in Skill %s"), *SkillID.ToString());
				continue;
			}
			
			TSubclassOf<UGameplayAbility> AbilityClass = SoftAbilityClass.LoadSynchronous();
			
			if (!AbilityClass)
			{
				LR_ERROR(TEXT("Failed to load Ability Class for Skill %s"), *SkillID.ToString());
				continue;
			}
			
			FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
			CharacterAbilityHandles.Add(Handle);
		}
	}
}

void ALRPlayerState::GrantEquipmentAbilities(EEquipmentSlotType Slot, FName EquipmentID)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem || !AbilitySystemComponent) return;

	//const FCharacterStaticData& CharData = DataSubsystem->GetCharacterStaticData(CharacterID);

	// 장비 ID로 스킬 ID 목록 조회
	TArray<FName> SkillIDs = DataSubsystem->GetEquipmentSkillIDs(EquipmentID);

	TArray<FGameplayAbilitySpecHandle> NewHandles;

	for (FName SkillID : SkillIDs)
	{
		const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(SkillID);

		for (TSoftClassPtr<UGameplayAbility> SoftAbilityClass : SkillData.GrantedAbilities)
		{
			if (SoftAbilityClass.IsNull())
			{
				LR_WARN(TEXT("Invalid Soft Class Reference in Skill %s"), *SkillID.ToString());
			}
			
			TSubclassOf<UGameplayAbility> AbilityClass = SoftAbilityClass.LoadSynchronous();
			
			if (!AbilityClass)
			{
				LR_ERROR(TEXT("Failed to load Ability Class for Skill %s"), *SkillID.ToString());
			}
			
			FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
			NewHandles.Add(Handle);
		}
	}

	// 핸들 저장 (나중에 장비 해제할 때 제거용)
	EquipmentAbilityHandles.Add(Slot, NewHandles);
}

void ALRPlayerState::ActivateSkill1()
{
	if (!AbilitySystemComponent) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem) return;

	TArray<FName> SkillIDs = DataSubsystem->GetCharacterSkillIDs(CharacterID);

	//(260226) KHS 수정. IF문 지옥좀 만들지마. TryActivateAbilityByClass쓰지마 ㅡㅡ
	if (!SkillIDs.IsValidIndex(0))
	{
		LR_WARN(TEXT("Skill1 발동 실패: DT에 등록된 캐릭터 스킬이 없음."));
		return;
	}
	
	//DT에서 스킬 태그 읽어서 전역 이벤트로 발동시키기
	FName TargetSkillID = SkillIDs[0];
	const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(TargetSkillID);

	FGameplayTag TriggerTag = SkillData.SkillTag;

	// 태그가 DT에 안 비어있는지 방어 코드 
	if (!TriggerTag.IsValid())
	{
		LR_WARN(TEXT("Skill1 발동 실패: DT에 스킬 트리거 태그가 세팅되지 않음. ID: %s"), *TargetSkillID.ToString());
		return;
	}

	//Instigator정보와 Target정보를 이벤트로 등록
	FGameplayEventData EvenData;
	EvenData.Instigator = Cast<const AActor>(GetPawn());
	EvenData.Target = nullptr; //직선형은 타겟 정보 불필요.
	
	LR_INFO(TEXT("[ActivateSkill1] Event 발송 시도 - Tag: %s, Instigator: %s"),
		*TriggerTag.ToString(), GetPawn() ? *GetPawn()->GetName() : TEXT("NULL"));
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Cast<AActor>(GetPawn()), TriggerTag, EvenData);
	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Cast<AActor>(GetPawn()), LRTags::Ability_Skill_Fireball, EvenData);
	
	LR_INFO(TEXT("[ActivateSkill1] Event 발송 완료 (GA 발동 여부는 GA 내부 로그 확인)"));
}

void ALRPlayerState::ActivateSkill2()
{
	// TODO_BJM: 나중에 무기 장착 시 무기 스킬(Skill2)을 꺼내서 발동하도록 구현

	if (!AbilitySystemComponent) return;

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem) return;

	// 현재 장착 중인 무기 ID 가져오기
	if (!EquippedItems.Contains(EEquipmentSlotType::WEAPON))
	{
		LR_WARN(TEXT("Skill2 발동 실패: 장착된 무기가 없음."));
		return;
	}
	FName WeaponID = EquippedItems[EEquipmentSlotType::WEAPON];

	// 무기 ID로 장비 스킬 ID 목록 가져오기
	TArray<FName> SkillIDs = DataSubsystem->GetEquipmentSkillIDs(WeaponID);

	if (!SkillIDs.IsValidIndex(0))
	{
		LR_WARN(TEXT("Skill2 발동 실패: 무기(%s)에 등록된 스킬이 없음."), *WeaponID.ToString());
		return;
	}

	// 스킬 데이터에서 태그 빼오기
	FName TargetSkillID = SkillIDs[0];
	const FSkillStaticData& SkillData = DataSubsystem->GetSkillStaticData(TargetSkillID);

	FGameplayTag TriggerTag = SkillData.SkillTag;

	if (!TriggerTag.IsValid())
	{
		LR_WARN(TEXT("Skill2 발동 실패: 무기 스킬(%s)에 태그가 없음."), *TargetSkillID.ToString());
		return;
	}

	// 이벤트 태그 발송 방식으로 실행
	FGameplayEventData EventData;
	EventData.Instigator = Cast<const AActor>(GetPawn());
	EventData.Target = nullptr;

	LR_INFO(TEXT("[ActivateSkill2] 무기 스킬 발송 시도 - Tag: %s"), *TriggerTag.ToString());

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Cast<AActor>(GetPawn()), TriggerTag, EventData);

	LR_INFO(TEXT("[ActivateSkill2] 무기 스킬 발송 완료"));
}

// (260303) KWB GameState로부터 에테르를 받는 함수
void ALRPlayerState::OnAetherReceived(float Amount)
{
	if (!AttributeSet)
	{
		LR_ERROR(TEXT("[PlayerState] AttributeSet이 null입니다"));
		return;
	}

	if (Amount <= 0.0f)
	{
		return;
	}

	// Aether 속성 증가
	float CurrentAether = AttributeSet->GetAether();
	float NewAether = CurrentAether + Amount;

	AttributeSet->SetAether(NewAether);
}
