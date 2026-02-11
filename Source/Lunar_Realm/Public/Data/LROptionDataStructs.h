// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LROptionDataStructs.generated.h"

USTRUCT(BlueprintType)
struct FSoundOptionData
{
	GENERATED_BODY()

public:
	void ApplyDefaults();
	//void UpdateAll(FSoundOptionData& Data);	// '=' 연산자로 대체 가능

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Sound")
	float MasterVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Sound")
	float BGMVolume = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Sound")
	float SFXVolume = 1.f;
};

USTRUCT(BlueprintType)
struct FGraphicOptionData
{
	GENERATED_BODY()

public:
	void ApplyDefaults();

	// 0: Low, 1: Medium, 2: High
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	int32 TextureQuality = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	int32 ShadowQuality = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	int32 AntiAliasingQuality = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	int32 PostProcessingQuality = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	int32 VisualEffectQuality = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	float ResolutionScale = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LR|Graphics")
	float FrameRateLimit = 60.f;

};
