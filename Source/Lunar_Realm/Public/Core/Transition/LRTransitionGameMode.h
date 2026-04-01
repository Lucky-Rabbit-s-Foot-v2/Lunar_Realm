// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/LRGameModeBase.h"
#include "Engine/StreamableManager.h"
#include "LRTransitionGameMode.generated.h"

// =============================================================================
/**
 * 비동기 레벨 전환용 게임모드
 */
//=============================================================================
// (260205) PJB 제작.
// (260219) PJB 수정. HUD 제거, 로딩 화면 소스 코드 이관.
// (260325) BJM 수정. 에셋 비동기로드 및 레벨 스트리밍 적용
// (260325) KWB 수정. Enemy 에셋 프리로드
//=============================================================================


UCLASS()
class LUNAR_REALM_API ALRTransitionGameMode : public ALRGameModeBase
{
	GENERATED_BODY()
	
	
	// (260325) BJM 추가
public:
	ALRTransitionGameMode();
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnLevelPreloadCompleted();

	void PreloadAssetsAsync();

	void StartLevelStreaming();

private:
	void GatherCharacterAssets(class USaveGameSubsystem* InSaveSys, class UGameDataSubsystem* InDataSys, TArray<FSoftObjectPath>& OutAssetsToLoad);
	void GatherEnemyAssets(class UGameInstance* InGI, class UGameDataSubsystem* InDataSys, TArray<FSoftObjectPath>& OutAssetsToLoad);
	void GatherChapterAndStageAssets(class UGameDataSubsystem* InDataSys, TArray<FSoftObjectPath>& OutAssetsToLoad);
protected:
	UPROPERTY(EditAnywhere, Category = "LR|Loading|Preload")
	float PreloadDuration = 3.f;

private:
	FName TargetLevelName = NAME_None;
	
	// (260325) BJM 추가
	bool bIsLoadingLevel = false;

	UPROPERTY()
	class ULRLoadingPageWidget* LoadingWidget;

	float CurrentFakeProgress = 0.0f;
	float LoadTime = 1.0f;

protected:
	TSharedPtr<struct FStreamableHandle> PreloadHandle;

};
