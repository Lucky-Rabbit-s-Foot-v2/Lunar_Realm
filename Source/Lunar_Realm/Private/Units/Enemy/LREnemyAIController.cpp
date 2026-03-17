// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"


ALREnemyAIController::ALREnemyAIController()
{
	HostileRootTag = LRTags::Team_Player;
	TargetCoreTag = LRTags::Team_Player_Structure_Core;
}

FGameplayTag ALREnemyAIController::TryAttackTarget(AActor* Target)
{
	if (!Target)
	{
		return FGameplayTag();
	}

	APawn* MyPawn = GetPawn();
	if (!MyPawn)
	{
		return FGameplayTag();
	}

	ALRCharacter* OwnerCharacter = Cast<ALRCharacter>(MyPawn);
	if (!OwnerCharacter)
	{
		return FGameplayTag();
	}

	UAbilitySystemComponent* ASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);
	if (!ASC)
	{
		return FGameplayTag();
	}

	if (!CachedNormalSkillTag.IsValid())
	{
		LR_WARN(TEXT("[%s] TryAttackTarget: CachedNormalSkillTag가 유효하지 않습니다."), *GetName());
		return FGameplayTag();
	}

	// TEST
	LR_INFO(TEXT("[TryAttackTarget] NormalTag: %s / SpecialTag: %s / bHasSpecial: %s"),
		*CachedNormalSkillTag.ToString(),
		*CachedSpecialSkillTag.ToString(),
		bHasSpecialSkill ? TEXT("TRUE") : TEXT("FALSE"));
	LR_INFO(TEXT("[TryAttackTarget] Normal 쿨타임: %s / Special 쿨타임: %s"),
		IsSkillOnCooldown(ASC, CachedNormalSkillTag) ? TEXT("ON") : TEXT("OFF"),
		IsSkillOnCooldown(ASC, CachedSpecialSkillTag) ? TEXT("ON") : TEXT("OFF"));

	// 우선순위 기반 스킬 + 몽타주 선택
	FGameplayTag SelectedSkillTag;
	UAnimMontage* SelectedMontage = nullptr;

	if (!IsSkillOnCooldown(ASC, CachedNormalSkillTag))
	{
		// Normal 사용 가능 → Normal 선택
		SelectedSkillTag = CachedNormalSkillTag;
		SelectedMontage = CachedNormalMontage;
	}
	else if (bHasSpecialSkill && !IsSkillOnCooldown(ASC, CachedSpecialSkillTag))
	{
		// Normal 쿨타임 중, Special 사용 가능 → Special 선택
		SelectedSkillTag = CachedSpecialSkillTag;
		SelectedMontage = CachedSpecialMontage;
	}
	else
	{
		// 둘 다 쿨타임 중 → Normal을 그냥 시도, GAS가 쿨타임으로 실패 처리
		// → LRBTTAttack의 OnAbilityFailed가 쿨타임 대기를 자동으로 처리함
		SelectedSkillTag = CachedNormalSkillTag;
		SelectedMontage = CachedNormalMontage;
	}

	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = Target;
	EventData.OptionalObject = SelectedMontage; // GA에서 꺼내 몽타주 재생에 사용

	// TEST
	LR_INFO(TEXT("[TryAttackTarget] 선택된 스킬 태그: %s"), *SelectedSkillTag.ToString());

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerCharacter, SelectedSkillTag, EventData);

	return SelectedSkillTag;
}

void ALREnemyAIController::InitializeFromEnemyData(FName EnemyID)
{
	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr;
	if (!DataSys)
	{
		LR_WARN(TEXT("[%s] InitializeFromEnemyData: No valid GameDataSubsystem"), *GetName());
		return;
	}

	const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(EnemyID);
	AttackRange = EnemyData.AttackRange;
	DetectionRadius = AttackRange + DetectionRadiusOffset;

	CachedNormalMontage = nullptr;
	CachedSpecialMontage = nullptr;
	CachedNormalSkillTag = FGameplayTag();
	CachedSpecialSkillTag = FGameplayTag();
	bHasSpecialSkill = false;

	if (EnemyData.SkillIDs.Num() < 1)
	{
		LR_WARN(TEXT("[%s] InitializeFromEnemyData: SkillIDs가 비어 있습니다."), *GetName());
		return;
	}

	// TEST
	// Normal 스킬 태그 + 몽타주 캐싱
	const FSkillStaticData& NormalSkillData = DataSys->GetSkillStaticData(EnemyData.SkillIDs[0]);
	CachedNormalSkillTag = NormalSkillData.SkillTag;

	if (!EnemyData.AttackNormalMontage.IsNull())
	{
		CachedNormalMontage = EnemyData.AttackNormalMontage.LoadSynchronous();
		if (!CachedNormalMontage)
		{
			LR_WARN(TEXT("[%s] AttackNormalMontage 로드 실패"), *GetName());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] AttackNormalMontage가 DT에 없습니다."), *GetName());
	}

	// Special 스킬 유효성 검사
	if (EnemyData.SkillIDs.Num() < 2)
	{
		LR_WARN(TEXT("[%s] SkillIDs가 1개 — Special 없이 Normal만 사용합니다."), *GetName());
		return;
	}

	if (EnemyData.AttackSpecialMontage.IsNull())
	{
		LR_WARN(TEXT("[%s] AttackSpecialMontage가 DT에 없음 — Normal만 사용합니다."), *GetName());
		return;
	}

	// Special 스킬 태그 + 몽타주 캐싱
	const FSkillStaticData& SpecialSkillData = DataSys->GetSkillStaticData(EnemyData.SkillIDs[1]);
	CachedSpecialSkillTag = SpecialSkillData.SkillTag;
	CachedSpecialMontage = EnemyData.AttackSpecialMontage.LoadSynchronous();

	if (!CachedSpecialMontage)
	{
		LR_WARN(TEXT("[%s] AttackSpecialMontage 로드 실패 — Normal만 사용합니다."), *GetName());
		return;
	}

	bHasSpecialSkill = true;
}

void ALREnemyAIController::OnPoolDeactivate_Implementation()
{
	// 부모의 BT 중단, BB 정리, UnPossess, Tick 비활성화를 먼저 실행
	Super::OnPoolDeactivate_Implementation();

	// Enemy 전용 캐싱 데이터 리셋
	CachedNormalMontage = nullptr;
	CachedSpecialMontage = nullptr;
	CachedNormalSkillTag = FGameplayTag();
	CachedSpecialSkillTag = FGameplayTag();
	bHasSpecialSkill = false;
}

bool ALREnemyAIController::IsSkillOnCooldown(UAbilitySystemComponent* ASC, FGameplayTag SkillTag) const
{
	if (!ASC || !SkillTag.IsValid())
	{
		return false;
	}

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.Ability)
		{
			continue;
		}

		// SkillTag를 AssetTag로 가진 GA를 탐색
		if (Spec.Ability->GetAssetTags().HasTag(SkillTag))
		{
			const FGameplayTagContainer* CooldownTags = Spec.Ability->GetCooldownTags();
			if (CooldownTags && ASC->HasAnyMatchingGameplayTags(*CooldownTags))
			{
				return true;
			}
			return false;
		}
	}

	// 해당 SkillTag를 가진 GA를 찾지 못한 경우
	return false;
}
