// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "LRStatusBarSlot.generated.h"

class UProgressBar;
class UImage;
class UWidgetAnimation;
class UAbilitySystemComponent;
class UTextBlock;

//=============================================================================
// (260324) KHS 제작. HUD 하위 StatusBar 슬롯 위젯.
// - 단일 대상(PlayerCore / EnemyCore / BossCharacter)의 HP, GhostHealth, 아이콘 표시
// - LRHealthWidget 패턴 참조
//=============================================================================

UENUM(BlueprintType)
enum class ESlotTargetType : uint8
{
	None,
	PlayerCore,
	EnemyCore,
	BossCharacter,
};

UCLASS()
class LUNAR_REALM_API ULRStatusBarSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	
	// 슬롯 초기화 진입점. ListWidget에서 호출.
	UFUNCTION(BlueprintCallable, Category = "LR|UI")
	void SetupSlot(ESlotTargetType InTargetType, UAbilitySystemComponent* InASC, UTexture2D* InIcon);

private:
	void BindToASC(UAbilitySystemComponent* InASC);

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	void UpdateGhostBar(float InDeltaTime);
	void CheckLowHealthState();
	void PlayHitEffect();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PBar_Health;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PBar_Ghost;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_ObjectName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Img_DangerGlow;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Anim_LowHealth;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Anim_HitShake;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	ESlotTargetType TargetType = ESlotTargetType::None;

	float TargetHealth     = 0.0f;
	float CurrentMaxHealth = 1.0f;
	float GhostHealth      = 0.0f;
	float GhostDelayTimer  = 0.0f;
	bool  bIsLowHealth     = false;

	static constexpr float GhostDelayTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	float InterpSpeed_Ghost = 3.0f;
};
