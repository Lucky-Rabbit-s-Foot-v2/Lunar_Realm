// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/LRAttributeSet.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/Actor.h"

void ULRAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void ULRAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetDamageAttribute())
	{
		return;
	}

	float LocalDamage = GetDamage();
	SetDamage(0.0f);

	if (LocalDamage == 0.0f) return;

	// 공통 체력 계산 (힐이든 데미지든 무조건 적용)
	float NewHealth = GetHealth() - LocalDamage;
	SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

	// 부모가 직접 데미지/힐 텍스트 띄우기
	if (AActor* AvatarActor = GetOwningAbilitySystemComponent()->GetAvatarActor())
	{
		if (UGameInstance* GI = AvatarActor->GetGameInstance())
		{
			if (UUIManagerSubsystem* UIManager = GI->GetSubsystem<UUIManagerSubsystem>())
			{
				FVector HitLocation = AvatarActor->GetActorLocation() + FVector(0.f, 0.f, 50.f);

				// 색상 결정
				FLinearColor TextColor = GetDamageTextColor(LocalDamage);

				// 절대값 처리
				float DisplayValue = FMath::Abs(LocalDamage);

				UIManager->ShowDamageText(DisplayValue, HitLocation, TextColor);
			}
		}
	}
	OnDamageExecuted(LocalDamage, Data);

}
