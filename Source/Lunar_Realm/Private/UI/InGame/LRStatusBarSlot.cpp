// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/InGame/LRStatusBarSlot.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "System/LoggingSystem.h"

void ULRStatusBarSlot::SetupSlot(ESlotTargetType InTargetType, UAbilitySystemComponent* InASC, UTexture2D* InIcon)
{
	if (!InASC)
	{
		LR_WARN(TEXT("SetupSlot: InASC is null. Slot will not be bound."));
		return;
	}

	TargetType = InTargetType;
	BindToASC(InASC);

	if (InIcon && Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
	}

	if (Txt_ObjectName)
	{
		FText Name = FText::GetEmpty();
		switch (InTargetType)
		{
			case ESlotTargetType::PlayerCore:    Name = FText::FromString(TEXT("아군기지")); break;
			case ESlotTargetType::EnemyCore:     Name = FText::FromString(TEXT("적 기지"));  break;
			case ESlotTargetType::BossCharacter: Name = FText::FromString(TEXT("보스"));     break;
			default: break;
		}
		Txt_ObjectName->SetText(Name);
	}
}

void ULRStatusBarSlot::BindToASC(UAbilitySystemComponent* InASC)
{
	CachedASC = InASC;

	TargetHealth     = CachedASC->GetNumericAttribute(ULRAttributeSet::GetHealthAttribute());
	CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());
	GhostHealth      = TargetHealth;

	if (PBar_Health)
	{
		float Percent = (CurrentMaxHealth > 0.0f) ? (TargetHealth / CurrentMaxHealth) : 0.0f;
		PBar_Health->SetPercent(Percent);
	}

	if (PBar_Ghost)
	{
		float Percent = (CurrentMaxHealth > 0.0f) ? (GhostHealth / CurrentMaxHealth) : 0.0f;
		PBar_Ghost->SetPercent(Percent);
	}

	CachedASC->GetGameplayAttributeValueChangeDelegate(ULRAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ULRStatusBarSlot::OnHealthChanged);

	CachedASC->GetGameplayAttributeValueChangeDelegate(ULRAttributeSet::GetMaxHealthAttribute())
		.AddUObject(this, &ULRStatusBarSlot::OnMaxHealthChanged);
}

void ULRStatusBarSlot::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateGhostBar(InDeltaTime);
}

void ULRStatusBarSlot::NativeDestruct()
{
	if (CachedASC)
	{
		CachedASC->GetGameplayAttributeValueChangeDelegate(ULRAttributeSet::GetHealthAttribute())
			.RemoveAll(this);
		CachedASC->GetGameplayAttributeValueChangeDelegate(ULRAttributeSet::GetMaxHealthAttribute())
			.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void ULRStatusBarSlot::OnHealthChanged(const FOnAttributeChangeData& Data)
{

	float OldHealth = TargetHealth;
	TargetHealth = Data.NewValue;

	if (FMath::IsNearlyEqual(OldHealth, TargetHealth, 0.01f))
	{
		return;
	}

	if (CachedASC)
	{
		CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());
	}

	// 체력 감소: 메인 바 즉시 갱신, Ghost 바는 딜레이 후 보간
	PlayHitEffect();

	if (PBar_Health)
	{
		float Percent = (CurrentMaxHealth > 0.0f) ? (TargetHealth / CurrentMaxHealth) : 0.0f;
		PBar_Health->SetPercent(Percent);
	}

	GhostDelayTimer = GhostDelayTime;

	CheckLowHealthState();
}

void ULRStatusBarSlot::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
}

void ULRStatusBarSlot::UpdateGhostBar(float InDeltaTime)
{
	if (GhostDelayTimer > 0.0f)
	{
		GhostDelayTimer -= InDeltaTime;
		return;
	}

	if (PBar_Ghost && !FMath::IsNearlyEqual(GhostHealth, TargetHealth, 0.1f))
	{
		GhostHealth = FMath::FInterpTo(GhostHealth, TargetHealth, InDeltaTime, InterpSpeed_Ghost);

		float Percent = (CurrentMaxHealth > 0.0f) ? (GhostHealth / CurrentMaxHealth) : 0.0f;
		PBar_Ghost->SetPercent(Percent);
	}
}

void ULRStatusBarSlot::CheckLowHealthState()
{
	if (CurrentMaxHealth <= 0.0f)
	{
		return;
	}

	float HealthRatio = TargetHealth / CurrentMaxHealth;
	bool bShouldBeLowHealth = (HealthRatio <= 0.3f) && (TargetHealth > 0.0f);

	if (bShouldBeLowHealth != bIsLowHealth)
	{
		bIsLowHealth = bShouldBeLowHealth;

		if (bIsLowHealth)
		{
			if (Img_DangerGlow)
			{
				Img_DangerGlow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			if (Anim_LowHealth)
			{
				PlayAnimation(Anim_LowHealth, 0.0f, 0);
			}
		}
		else
		{
			if (Img_DangerGlow)
			{
				Img_DangerGlow->SetVisibility(ESlateVisibility::Hidden);
			}
			if (Anim_LowHealth)
			{
				StopAnimation(Anim_LowHealth);
			}
		}
	}
}

void ULRStatusBarSlot::PlayHitEffect()
{
	if (Anim_HitShake)
	{
		PlayAnimation(Anim_HitShake);
	}
}
