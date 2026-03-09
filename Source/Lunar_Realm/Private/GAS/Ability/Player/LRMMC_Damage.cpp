// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Player/LRMMC_Damage.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"


ULRMMC_Damage::ULRMMC_Damage()
{
	// Source의 AttackPower를 캡처하겠다고 세팅
	AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(ULRAttributeSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);

	// 캡처 목록에 등록
	RelevantAttributesToCapture.Add(AttackPowerDef);
}

float ULRMMC_Damage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& InSpec) const
{
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = InSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = InSpec.CapturedTargetTags.GetAggregatedTags();

	float AttackPower = 0.0f;
	GetCapturedAttributeMagnitude(AttackPowerDef, InSpec, EvaluationParameters, AttackPower);

	// 오차범위 적용 로직
	float MinDamage = AttackPower * 0.85f;
	float MaxDamage = AttackPower * 1.15f;

	return FMath::RandRange(MinDamage, MaxDamage);
}
