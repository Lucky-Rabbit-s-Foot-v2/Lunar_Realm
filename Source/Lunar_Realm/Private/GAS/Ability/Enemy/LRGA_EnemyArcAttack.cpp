#include "GAS/Ability/Enemy/LRGA_EnemyArcAttack.h"
#include "GAS/Tags/LRGameplayTags.h"

ULRGA_EnemyArcAttack::ULRGA_EnemyArcAttack()
{
	// 부모의 태그를 덮어쓰기
	// NOTE: Blueprint CDO가 C++ 생성자의 SetAssetTags()를 덮어쓰므로
	//       실제 AssetTag는 BP Class Defaults에서 설정해야 함
	FGameplayTagContainer TempTags = GetAssetTags();
	TempTags.RemoveTag(LRTags::Ability_Skill_EnemyRangedShoot);
	TempTags.AddTag(LRTags::Ability_Skill_EnemyArcShoot); // 새 태그 필요
	SetAssetTags(TempTags);

	// 트리거 재설정
	AbilityTriggers.Empty();
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_EnemyArcShoot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	// DT 참조 키 — 에너미별 BP에서 오버라이드 가능
	SkillID = "SKILL_ENEMY_ARC_SHOOT";
	SkillEffectID = "EFFECT_ENEMY_ARC_SHOOT";

	CooldownTagContainer.Reset(1);
	CooldownTagContainer.AddTag(
		FGameplayTag::RequestGameplayTag(FName("Ability.Skill.EnemyArcShoot.Cooldown")));
}