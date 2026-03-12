// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Player/LRPlayerCameraManager.h"
#include "Units/Player/LRPlayerCharacter.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

ALRPlayerCameraManager::ALRPlayerCameraManager()
{
	//FixedRotation = FRotator(-30.0f, 0.0f, 0.0f);
	FixedRotation = FRotator(-20.0f, 0.0f, 0.0f);
	
	// 카메라 위아래
	//FixedX = -688.0f;
	//FixedZ = 700.0f;
	FixedX = -550.0f;
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

	OutVT.POV.ProjectionMode = ECameraProjectionMode::Orthographic;
	//OutVT.POV.OrthoWidth = 1200.0f;
	OutVT.POV.OrthoWidth = 900.0f;

	//OutVT.POV.FOV = 90.0f;

}
