// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRHealthWidget.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"

void ULRHealthWidget::BindToASC(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	CachedASC = ASC;

	bool bFoundHealth = false;
	bool bFoundMaxHealth = false;

	CurrentHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetHealthAttribute());
	CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());

	UpdateHealth(CurrentHealth, CurrentMaxHealth);

	CachedASC->GetGameplayAttributeValueChangeDelegate(ULRAttributeSet::GetHealthAttribute())
		.AddUObject(this, &ULRHealthWidget::OnHealthChanged);

	CachedASC->GetGameplayAttributeValueChangeDelegate(ULRAttributeSet::GetMaxHealthAttribute())
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
	if (CachedASC)
	{
		CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());
	}
	UpdateHealth(CurrentHealth, CurrentMaxHealth);
}

void ULRHealthWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	if (CachedASC)
	{
		CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetHealthAttribute());
	}
	UpdateHealth(CurrentHealth, CurrentMaxHealth);
}

void ULRHealthWidget::UpdatePlayerIcon(FName InCharacterID)
{
	if (!Img_PlayerIcon)
	{
		return;
	}
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSubsystem) return;

	const FCharacterStaticData& CharData = DataSubsystem->GetCharacterStaticData(InCharacterID);

	if (!CharData.CharacterTexture.IsNull())
	{
		UTexture2D* LoadedIcon = CharData.CharacterTexture.LoadSynchronous();
		if (LoadedIcon)
		{
			Img_PlayerIcon->SetBrushFromTexture(LoadedIcon);
		}

		if (Text_PlayerName)
		{
			Text_PlayerName->SetText(FText::FromString(CharData.CharacterName));
		}
	}
}
