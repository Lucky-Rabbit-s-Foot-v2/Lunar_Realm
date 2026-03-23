// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LRPlayerCameraManager.generated.h"

/**
 * 횡스크롤 스타일 카메라 매니저 클래스
 */

//=============================================================================
// (260210) BJM 제작. 카메라 매니저.
// (260303) BJM 수정. 카메라 위치 고정, Y축 범위 설정
//=============================================================================
UCLASS()
class LUNAR_REALM_API ALRPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:

	ALRPlayerCameraManager();

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

protected:

	bool bIsInitialized = false;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Limit")
	float MinY = -10000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Limit")
	float MaxY = 10000.0f;

	float CapturedX = 0.0f;
	float CapturedZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Setting")
	float OffsetX = 1100.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Setting")
	float OffsetZ = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Setting")
	float CameraYaw = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Setting")
	float CameraPitch = -20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Setting")
	float CameraLagSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Setting")
	float FixedX = -1000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Setting")
	float FixedZ = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Setting")
	FRotator FixedRotation = FRotator(-45.0f, 0.0f, 0.0f);

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SetStageCameraBounds(float InMinY, float InMaxY);

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ResetCameraInitialization();

private:
	float StageMinY = -999999.0f;
	float StageMaxY = 999999.0f;
};
