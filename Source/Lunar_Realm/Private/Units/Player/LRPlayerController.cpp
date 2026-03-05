// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"
#include "Units/Player/LRPlayerState.h"
#include "GameFramework/TouchInterface.h"
#include "Units/Player/LRPlayerCharacter.h"

#include "UI/InGame/LRPlayerWidget.h"


#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"


ALRPlayerController::ALRPlayerController()
{
	PlayerCameraManagerClass = ALRPlayerCameraManager::StaticClass();
}

void ALRPlayerController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUI<ULRPlayerWidget>(PlayerWidget);
}

void ALRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (MobileTouchInterface)
		{
			ActivateTouchInterface(MobileTouchInterface);
			SetVirtualJoystickVisibility(true);
			LR_WARN(TEXT("터치 인터페이스 활성화 성공"));
		}
		else
		{
			LR_ERROR(TEXT("터치 인터페이스 활성화 실패"));
		}
	}
}

void ALRPlayerController::EquipWeapon1()
{
	if (ALRPlayerState* PS = GetPlayerState<ALRPlayerState>())
	{
		FName WeaponID = FName("EQUIP_MELEE_06");
		PS->EquipItem(EEquipmentSlotType::WEAPON, WeaponID);

		// DT에서 무기 이름 꺼내오기
		FString EqName = TEXT("Unknown");
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>())
			{
				const FEquipmentStaticData& EquipData = DataSubsystem->GetEquipmentStaticData(WeaponID);
				EqName = EquipData.EquipmentName;
			}
		}

		LR_INFO(TEXT("[PlayerController] 장착 명령 전송 완료 - 무기 1번: %s"), *EqName);
	}
}

void ALRPlayerController::EquipWeapon2()
{
	if (ALRPlayerState* PS = GetPlayerState<ALRPlayerState>())
	{
		FName WeaponID = FName("EQUIP_MELEE_07");
		PS->EquipItem(EEquipmentSlotType::WEAPON, WeaponID);

		FString EqName = TEXT("Unknown");
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>())
			{
				const FEquipmentStaticData& EquipData = DataSubsystem->GetEquipmentStaticData(WeaponID);
				EqName = EquipData.EquipmentName;
			}
		}

		LR_INFO(TEXT("[PlayerController] 장착 완료 - 무기 2번: %s"), *EqName);
	}
}

void ALRPlayerController::EquipWeapon3()
{
	if (ALRPlayerState* PS = GetPlayerState<ALRPlayerState>())
	{
		FName WeaponID = FName("EQUIP_HELMET_08");
		PS->EquipItem(EEquipmentSlotType::WEAPON, WeaponID);

		FString EqName = TEXT("Unknown");
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UGameDataSubsystem* DataSubsystem = GI->GetSubsystem<UGameDataSubsystem>())
			{
				const FEquipmentStaticData& EquipData = DataSubsystem->GetEquipmentStaticData(WeaponID);
				EqName = EquipData.EquipmentName;
			}
		}

		LR_INFO(TEXT("[PlayerController] 장착 완료 - 무기 3번: %s"), *EqName);
	}
}

void ALRPlayerController::UnequipWeapon()
{
	if (ALRPlayerState* PS = GetPlayerState<ALRPlayerState>())
	{
		PS->UnequipItem(EEquipmentSlotType::WEAPON);
		LR_INFO(TEXT("[PlayerController] 무기 장착 해제 명령 전송 완료"));
	}
}

void ALRPlayerController::OnPlayerDied(float InRespawnTime)
{
	if (ULRPlayerWidget* MyWidget = GetPlayerWidget())
	{
		MyWidget->UpdateUIOnDeath(true, InRespawnTime);
	}

	SetVirtualJoystickVisibility(false);
	SetScreenGrayscale(1.0f);
}

void ALRPlayerController::OnPlayerRespawned()
{
	if (ULRPlayerWidget* MyWidget = GetPlayerWidget())
	{
		MyWidget->UpdateUIOnDeath(false);
	}

	SetVirtualJoystickVisibility(true);
	SetScreenGrayscale(0.0f);
}

void ALRPlayerController::SetScreenGrayscale(float InWeight)
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), FoundVolumes);

	for (AActor* VolumeActor : FoundVolumes)
	{
		if (APostProcessVolume* Volume = Cast<APostProcessVolume>(VolumeActor))
		{
			Volume->BlendWeight = InWeight;
		}
	}
}

void ALRPlayerController::ToggleAutoMode()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->ToggleAutoMode();
	}
}

void ALRPlayerController::UsePotion()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->UsePotion();
	}
}

void ALRPlayerController::UseSkill1()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		ALRPlayerState* MyState = MyCharacter->GetPlayerState<ALRPlayerState>();
		if (MyState)
		{
			MyState->ActivateSkill1();
		}
	}
}

void ALRPlayerController::UseSkill2()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		ALRPlayerState* MyState = MyCharacter->GetPlayerState<ALRPlayerState>();
		if (MyState)
		{
			MyState->ActivateSkill2();
		}
	}
}

UAbilitySystemComponent* ALRPlayerController::GetAbilitySystemComponent()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	return MyCharacter ? MyCharacter->GetAbilitySystemComponent() : nullptr;
}

ULRPlayerWidget* ALRPlayerController::GetPlayerWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	return UIManager->GetOrCreateWidget(PlayerWidget);
}
