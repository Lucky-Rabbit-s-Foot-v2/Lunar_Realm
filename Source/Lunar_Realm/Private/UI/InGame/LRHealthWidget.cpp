// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"

void ULRHealthWidget::BindToASC(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	CachedASC = ASC;

	bool bFoundHealth = false;
	bool bFoundMaxHealth = false;

	TargetHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetHealthAttribute());
	CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());

	CurrentVisualHealth = TargetHealth;
	GhostHealth = TargetHealth;

	UpdateHealth(CurrentVisualHealth, CurrentMaxHealth);

	if (PBar_Ghost)
	{
		float Percent = (CurrentMaxHealth > 0.0f) ? (GhostHealth / CurrentMaxHealth) : 0.0f;
		PBar_Ghost->SetPercent(Percent);
	}

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

void ULRHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateGhostBar(InDeltaTime);

	//if (!FMath::IsNearlyEqual(CurrentVisualHealth, TargetHealth, 0.1f))
	//{
	//	CurrentVisualHealth = FMath::FInterpTo(CurrentVisualHealth, TargetHealth, InDeltaTime, InterpSpeed);
	//	UpdateHealth(CurrentVisualHealth, CurrentMaxHealth);
	//}
}

void ULRHealthWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
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

	// 데미지를 입었을 때와 회복할 때를 구분!
	if (TargetHealth < OldHealth)
	{
		// 체력이 깎였으므로 피격 효과 실행!
		PlayHitEffect();

		// 메인 체력바는 즉시 깎아버림 (잔상 바는 틱에서 서서히 따라감)
		UpdateHealth(TargetHealth, CurrentMaxHealth);

		GhostDelayTimer = GhostDelayTime;
	}
	else if (TargetHealth > OldHealth)
	{
		GhostHealth = TargetHealth;
		GhostDelayTimer = 0.0f;
		UpdateHealth(TargetHealth, CurrentMaxHealth);

		if (PBar_Ghost)
		{
			float Percent = (CurrentMaxHealth > 0.0f) ? (TargetHealth / CurrentMaxHealth) : 0.0f;
			PBar_Ghost->SetPercent(Percent);
		}
	}

	// 체력 상태를 체크해서 30% 이하면 위기 연출 실행
	CheckLowHealthState();

	//TargetHealth = Data.NewValue;
	//if (CachedASC)
	//{
	//	CurrentMaxHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());
	//}
	////UpdateHealth(CurrentHealth, CurrentMaxHealth);
}

void ULRHealthWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	if (CachedASC)
	{
		TargetHealth = CachedASC->GetNumericAttribute(ULRAttributeSet::GetHealthAttribute());
	}
	//UpdateHealth(CurrentHealth, CurrentMaxHealth);
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

	// 프레임이 있을 때만 시도, 없으면 그냥 숨김
	if (Img_PortraitFrame && !CharData.PortraitFrame.IsNull())
	{
		UTexture2D* LoadedFrame = CharData.PortraitFrame.LoadSynchronous();
		if (LoadedFrame)
		{
			Img_PortraitFrame->SetBrushFromTexture(LoadedFrame);
			Img_PortraitFrame->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
	else if (Img_PortraitFrame)
	{
		Img_PortraitFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULRHealthWidget::StartRespawnTimer(float InRespawnTime)
{
	if (Text_RespawnTimer)
	{
		CurrentRespawnTime = InRespawnTime;
		Text_RespawnTimer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		UpdateRespawnTimerText();

		GetWorld()->GetTimerManager().SetTimer(UI_RespawnTimerHandle, this, &ULRHealthWidget::UpdateRespawnTimerText, 0.1f, true);
	}
}


void ULRHealthWidget::UpdateRespawnTimerText()
{
	CurrentRespawnTime -= 0.1f;

	if (CurrentRespawnTime <= 0.0f)
	{
		StopRespawnTimer();
	}
	else if (Text_RespawnTimer)
	{
		int32 DisplayTime = FMath::CeilToInt(CurrentRespawnTime);

		FString TimeString = FString::Printf(TEXT("%d"), DisplayTime);
		Text_RespawnTimer->SetText(FText::FromString(TimeString));
	}
}

void ULRHealthWidget::UpdateGhostBar(float InDeltaTime)
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

void ULRHealthWidget::CheckLowHealthState()
{
	if (CurrentMaxHealth <= 0.0f) return;

	float HealthRatio = TargetHealth / CurrentMaxHealth;
	bool bShouldBeLowHealth = (HealthRatio <= 0.3f) && (TargetHealth > 0.0f);

	// 상태가 변했을 때만 애니메이션 재생/정지
	if (bShouldBeLowHealth != bIsLowHealth)
	{
		bIsLowHealth = bShouldBeLowHealth;

		if (bIsLowHealth)
		{
			// 30% 이하일 때: 켜고 깜빡임 시작
			if (Img_DangerGlow) Img_DangerGlow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (Anim_LowHealth) PlayAnimation(Anim_LowHealth, 0.0f, 0);
		}
		else
		{
			// 30% 초과일 때: 아예 숨기고 애니메이션 정지
			if (Img_DangerGlow) Img_DangerGlow->SetVisibility(ESlateVisibility::Hidden);
			if (Anim_LowHealth) StopAnimation(Anim_LowHealth);
		}
	}
}

void ULRHealthWidget::PlayHitEffect()
{
	if (Anim_HitShake)
	{
		PlayAnimation(Anim_HitShake);
	}
}

void ULRHealthWidget::StopRespawnTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(UI_RespawnTimerHandle);

	if (Text_RespawnTimer)
	{
		Text_RespawnTimer->SetVisibility(ESlateVisibility::Collapsed);
	}
}