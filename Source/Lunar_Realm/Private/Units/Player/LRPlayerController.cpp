// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerController.h"
#include "Units/Player/LRPlayerCameraManager.h"
#include "Units/Player/LRPlayerState.h"
#include "GameFramework/TouchInterface.h"
#include "Units/Player/LRPlayerCharacter.h"

#include "UI/InGame/LRInGamePersistentWidget.h"
#include "UI/InGame/LRSkillCutInWidget.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"



ALRPlayerController::ALRPlayerController()
	: ALRControllerBase()
{
	PlayerCameraManagerClass = ALRPlayerCameraManager::StaticClass();
}

void ALRPlayerController::OpenFirstWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	UIManager->OpenUIByID(EUIID::INGAME);
	UIManager->HideBackgroundUI();
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
		FName WeaponID = FName("EQUIP_BLUNT_05");
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
		FName WeaponID = FName("EQUIP_STAFF_05");
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
		FName WeaponID = FName("EQUIP_SWORD_10");
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
	if (ULRInGamePersistentWidget* MyWidget = GetPlayerWidget())
	{
		MyWidget->UpdateUIOnDeath(true, InRespawnTime);
	}

	SetVirtualJoystickVisibility(false);
	SetScreenGrayscale(1.0f);
}

void ALRPlayerController::OnPlayerRespawned()
{
	if (ULRInGamePersistentWidget* MyWidget = GetPlayerWidget())
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

void ALRPlayerController::ToggleGameSpeed()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (!MyCharacter) return;

	if (!MyCharacter->IsAutoMode()) return;

	if (CurrentGameSpeed == 1.0f)
	{
		CurrentGameSpeed = 1.5f;
	}
	else if (CurrentGameSpeed == 1.5f)
	{
		CurrentGameSpeed = 2.0f;
	}
	else
	{
		CurrentGameSpeed = 1.0f;
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), CurrentGameSpeed);
	LR_INFO(TEXT("게임 배속 변경: x%.1f"), CurrentGameSpeed);

	if (ULRInGamePersistentWidget* MyWidget = GetPlayerWidget())
	{
		MyWidget->UpdateSpeedVisual(CurrentGameSpeed);
	}
}

void ALRPlayerController::ToggleAutoMode()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	if (MyCharacter)
	{
		MyCharacter->ToggleAutoMode();

		bool bCurrentAutoMode = MyCharacter->IsAutoMode();

		if (ULRInGamePersistentWidget* MyWidget = GetPlayerWidget())
		{
			MyWidget->UpdateAutoButtonVisual(bCurrentAutoMode);
			MyWidget->SetSpeedButtonLocked(!bCurrentAutoMode);

			if (!bCurrentAutoMode)
			{
				CurrentGameSpeed = 1.0f;
				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), CurrentGameSpeed);
				MyWidget->UpdateSpeedVisual(CurrentGameSpeed);
			}
		}
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
			FName CharID = MyState->GetCharacterID();

			if (CharID == FName("Writer_UR") || CharID == FName("Muzzle_UR") || CharID == FName("Nurse_UR"))
			{
				if (SkillCutInWidgetClass)
				{
					ULRSkillCutInWidget* CutInWidget = CreateWidget<ULRSkillCutInWidget>(this, SkillCutInWidgetClass);
					if (CutInWidget)
					{
						CutInWidget->InitCutIn(CharID);

						CutInWidget->AddToViewport(100);
						UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.01f);
					}
				}
			}
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

void ALRPlayerController::EndSkillCutIn(UUserWidget* InCutInWidget)
{
	if (InCutInWidget)
	{
		InCutInWidget->RemoveFromParent();
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), CurrentGameSpeed);
}

UAbilitySystemComponent* ALRPlayerController::GetAbilitySystemComponent()
{
	ALRPlayerCharacter* MyCharacter = Cast<ALRPlayerCharacter>(GetPawn());
	LR_WARN(TEXT("[PlayerController] GetAbilitySystemComponent called - Character: %s"), MyCharacter ? *MyCharacter->GetName() : TEXT("NULL"));
	return MyCharacter ? MyCharacter->GetAbilitySystemComponent() : nullptr;
}

ULRInGamePersistentWidget* ALRPlayerController::GetPlayerWidget()
{
	UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	return Cast<ULRInGamePersistentWidget>(UIManager->OpenUIByID(EUIID::INGAME));
}
