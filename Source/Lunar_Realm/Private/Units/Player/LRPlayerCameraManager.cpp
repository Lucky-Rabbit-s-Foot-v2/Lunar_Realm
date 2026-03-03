// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/LRPlayerCameraManager.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

ALRPlayerCameraManager::ALRPlayerCameraManager()
{
	FixedRotation = FRotator(-30.0f, 0.0f, 0.0f);
	
	// 카메라랑 플레이어랑 거리
	FixedX = -860.0f;
	FixedZ = 500.0f;  


}

void ALRPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	if (!OutVT.Target)
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	FVector TargetLoc = OutVT.Target->GetActorLocation();
	float CurrentCameraOffset = 0.0f;

	if (ALRPlayerCharacter* PlayerChar = Cast<ALRPlayerCharacter>(OutVT.Target))
	{
		CurrentCameraOffset = PlayerChar->GetCameraOffsetY();
	}

	float NewX = FixedX;

	float NewY = FMath::Clamp(TargetLoc.Y + CurrentCameraOffset, MinY, MaxY);

	float NewZ = FixedZ;

	FVector DesiredLoc = FVector(NewX, NewY, NewZ);
	FRotator DesiredRot = FixedRotation;

	if (CameraLagSpeed > 0.0f)
	{
		OutVT.POV.Location = FMath::VInterpTo(OutVT.POV.Location, DesiredLoc, DeltaTime, CameraLagSpeed);
	}
	else
	{
		OutVT.POV.Location = DesiredLoc;
	}

	OutVT.POV.Rotation = DesiredRot;
	OutVT.POV.FOV = 90.0f;

}
