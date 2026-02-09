// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRHealthWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"

void ULRHealthWidget::BindToASC(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("[HealthWidget] ASC가 없습니다! (NULL)"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[HealthWidget] BindToASC 실행됨! ASC 이름: %s"), *ASC->GetName());

	bool bFoundHealth = false;
	bool bFoundMaxHealth = false;

	CurrentHealth = ASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetHealthAttribute(), bFoundHealth);
	CurrentMaxHealth = ASC->GetGameplayAttributeValue(ULRPlayerAttributeSet::GetMaxHealthAttribute(), bFoundMaxHealth);

	UE_LOG(LogTemp, Log, TEXT("[HealthWidget] 초기 체력: %.1f / %.1f (찾음: %d)"), CurrentHealth, CurrentMaxHealth, bFoundHealth);
	UpdateHealth(CurrentHealth, CurrentMaxHealth);

	ASC->GetGameplayAttributeValueChangeDelegate(ULRPlayerAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ULRHealthWidget::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(ULRPlayerAttributeSet::GetMaxHealthAttribute())
		.AddUObject(this, &ULRHealthWidget::OnMaxHealthChanged);
}

void ULRHealthWidget::UpdateHealth(float InCurrentHealth, float InMaxHealth)
{
	if (PBar_Health)
	{
		float Percent = (InMaxHealth > 0.0f) ? (InCurrentHealth / InMaxHealth) : 0.0f;
		PBar_Health->SetPercent(Percent);
	}
}

void ULRHealthWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	UE_LOG(LogTemp, Log, TEXT("[HealthWidget] 체력 변경 감지! 새로운 값: %.1f"), CurrentHealth);
	UpdateHealth(CurrentHealth, CurrentMaxHealth);
}

void ULRHealthWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	UpdateHealth(CurrentHealth, CurrentMaxHealth);
}
