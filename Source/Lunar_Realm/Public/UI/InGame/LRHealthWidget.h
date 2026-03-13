// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "GameplayEffectTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "LRHealthWidget.generated.h"

/**
 * 
 */

//=============================================================================
// (260209) BJM 제작. Health UI 위젯 클래스 생성.
//=============================================================================

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class LUNAR_REALM_API ULRHealthWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void BindToASC(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void UpdateHealth(float InCurrentHealth, float InMaxHealth);

public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PBar_Health;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	//float CurrentHealth = 0.0f;
	//float CurrentMaxHealth = 1.0f;

protected:
	UPROPERTY()
	UAbilitySystemComponent* CachedASC;

public:
	void UpdatePlayerIcon(FName InCharacterID);


protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_PlayerIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_PlayerName;

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RespawnTimer;

	FTimerHandle UI_RespawnTimerHandle;
	float CurrentRespawnTime;

public:
	void StartRespawnTimer(float InRespawnTime);
	void StopRespawnTimer();

protected:
	void UpdateRespawnTimerText();

protected:
	float TargetHealth;
	float CurrentVisualHealth;
	float CurrentMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|UI")
	float InterpSpeed = 10.0f;

};
