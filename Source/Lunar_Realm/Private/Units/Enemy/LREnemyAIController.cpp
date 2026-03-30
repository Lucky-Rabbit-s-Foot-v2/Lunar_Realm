// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Enemy/LREnemyAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimMontage.h"

#include "BehaviorTree/BehaviorTree.h"

#include "Engine/GameInstance.h"

#include "GAS/Tags/LRGameplayTags.h"

#include "Subsystems/GameDataSubsystem.h"

#include "Units/Enemy/LREnemyCharacter.h"
#include "Units/LRCharacter.h"


ALREnemyAIController::ALREnemyAIController()
{
	HostileRootTag = LRTags::Team_Player;
	TargetCoreTag = LRTags::Team_Player_Structure_Core;
}

FGameplayTag ALREnemyAIController::TryAttackTarget(AActor* Target)
{
	// 일반/엘리트: 항상 Phase 0 (index 0, 1)으로 동작
	return TryAttackTargetByPhase(Target, 0);
}
FGameplayTag ALREnemyAIController::TryAttackTargetByPhase(AActor* Target, int32 Phase)
{
	// 파괴/풀복귀된 액터의 댕글링 포인터 방어
	if (!IsValid(Target))
	{
		return FGameplayTag();
	}

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

	// 페이즈에 따른 스킬 인덱스 산출
	const int32 NormalIdx = Phase * 2;
	const int32 SpecialIdx = NormalIdx + 1;

	// Normal 인덱스 유효성 체크
	if (!CachedSkillTags.IsValidIndex(NormalIdx))
	{
		LR_WARN(TEXT("[%s] TryAttackTargetByPhase: Phase %d에 해당하는 Normal 스킬(index %d)이 없음"),
			*GetName(), Phase, NormalIdx);
		return FGameplayTag();
	}

	// 우선순위 기반 스킬 + 몽타주 선택
	FGameplayTag SelectedTag;
	UAnimMontage* SelectedMontage = nullptr;

	if (!IsSkillOnCooldown(ASC, CachedSkillTags[NormalIdx]))
	{
		// Normal 사용 가능 → Normal 선택
		SelectedTag = CachedSkillTags[NormalIdx];
		SelectedMontage = CachedAttackMontages.IsValidIndex(NormalIdx) ? CachedAttackMontages[NormalIdx] : nullptr;
	}
	else if (CachedSkillTags.IsValidIndex(SpecialIdx) && !IsSkillOnCooldown(ASC, CachedSkillTags[SpecialIdx]))
	{
		// Normal 쿨타임 중, Special 사용 가능 → Special 선택
		SelectedTag = CachedSkillTags[SpecialIdx];
		SelectedMontage = CachedAttackMontages.IsValidIndex(SpecialIdx) ? CachedAttackMontages[SpecialIdx] : nullptr;
	}
	else
	{
		// 둘 다 쿨타임 중 → Normal을 그냥 시도, GAS가 쿨타임으로 실패 처리
		// → LRBTTAttack의 OnAbilityFailed가 쿨타임 대기를 자동으로 처리함
		SelectedTag = CachedSkillTags[NormalIdx];
		SelectedMontage = CachedAttackMontages.IsValidIndex(NormalIdx) ? CachedAttackMontages[NormalIdx] : nullptr;
	}

	FGameplayEventData EventData;
	EventData.Instigator = OwnerCharacter;
	EventData.Target = Target;
	EventData.OptionalObject = SelectedMontage;

	if (ALREnemyCharacter* EnemyChar = Cast<ALREnemyCharacter>(MyPawn))
	{
		int32 SelectedSkillIndex = (SelectedTag == CachedSkillTags[NormalIdx]) ? NormalIdx : SpecialIdx;
		EnemyChar->PlayAttackSound(SelectedSkillIndex);
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerCharacter, SelectedTag, EventData);

	return SelectedTag;
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

	// 배열 초기화
	CachedSkillTags.Empty();
	CachedAttackMontages.Empty();

	if (EnemyData.SkillIDs.Num() < 1)
	{
		LR_WARN(TEXT("[%s] InitializeFromEnemyData: SkillIDs가 비어 있습니다."), *GetName());
		return;
	}

	// SkillIDs 수만큼 루프: 스킬 태그 + 몽타주 캐싱
	for (int32 i = 0; i < EnemyData.SkillIDs.Num(); ++i)
	{
		// 스킬 태그 캐싱
		const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(EnemyData.SkillIDs[i]);
		CachedSkillTags.Add(SkillData.SkillTag);

		// 몽타주 캐싱
		UAnimMontage* LoadedMontage = nullptr;
		if (EnemyData.AttackMontages.IsValidIndex(i) && !EnemyData.AttackMontages[i].IsNull())
		{
			LoadedMontage = EnemyData.AttackMontages[i].LoadSynchronous();
			if (!LoadedMontage)
			{
				LR_WARN(TEXT("[%s] AttackMontages[%d] 로드 실패"), *GetName(), i);
			}
		}
		else
		{
			LR_WARN(TEXT("[%s] AttackMontages[%d]가 DT에 없거나 유효하지 않습니다."), *GetName(), i);
		}

		CachedAttackMontages.Add(LoadedMontage);
	}
}

void ALREnemyAIController::OnPoolDeactivate_Implementation()
{
	// 부모의 BT 중단, BB 정리, UnPossess, Tick 비활성화를 먼저 실행
	Super::OnPoolDeactivate_Implementation();

	// Enemy 전용 캐싱 데이터 리셋
	CachedSkillTags.Empty();
	CachedAttackMontages.Empty();
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
