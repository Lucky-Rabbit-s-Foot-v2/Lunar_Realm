// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRStatusBarListWidget.generated.h"

class ULRStatusBarSlot;
class ALRPlayerCore;
class ALREnemyCore;
class ALREnemyBossCharacter;
class ALRCore;
class ALREnemySpawner;

//=============================================================================
// (260324) KHS 제작. HUD 하위 StatusBar 리스트 위젯.
// - 스테이지 상태(IsBossStage)에 따라 0번(PlayerCore), 1번(EnemyCore/Boss) 슬롯 구성
// - OnBossSpawned 이벤트에 반응하여 슬롯 초기화
//=============================================================================

UCLASS()
class LUNAR_REALM_API ULRStatusBarListWidget : public ULRBaseWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// NativeConstruct에서 NextTick으로 지연 호출 — 코어 BeginPlay 완료 후 슬롯 초기화 보장
	void SetupSlotsDeferred();

	void SetupPlayerCoreSlot();
	void SetupEnemySlot();

	// OnBossSpawned 델리게이트 핸들러 — 보스 초기화 완료 후 즉시 슬롯 바인딩
	UFUNCTION()
	void OnBossSpawnedHandler(ALREnemyBossCharacter* BossChar);
	void BindBossSlot(ALREnemyBossCharacter* BossChar);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRStatusBarSlot> SlotUI_0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULRStatusBarSlot> SlotUI_1;

private:
	TWeakObjectPtr<ALRCore>               WeakPlayerCore;
	TWeakObjectPtr<ALRCore>               WeakEnemyCore;
	TWeakObjectPtr<ALREnemyBossCharacter> WeakBossChar;
	TWeakObjectPtr<ALREnemySpawner>       WeakSpawner;
};
