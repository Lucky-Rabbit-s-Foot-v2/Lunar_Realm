// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/Option/OptionManagerSubsystem.h"

#include "Engine/GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

#include "Subsystems/Option/GraphicSubsystem.h"
#include "Subsystems/Option/SoundSubsystem.h"

void UOptionManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UGraphicSubsystem>();
	Collection.InitializeDependency<USoundSubsystem>();

	Super::Initialize(Collection);
	LR_INFO(TEXT("OptionManager Initialized"));

	LoadOptions();

	InitializeOptions();
}

void UOptionManagerSubsystem::Deinitialize()
{
	SaveOptions();
	
	Super::Deinitialize();
}

void UOptionManagerSubsystem::UpdateCurrentOptionSaveGame()
{
	if (UGraphicSubsystem* GraphicSubsys = GetGameInstance()->GetSubsystem<UGraphicSubsystem>())
	{
		CurrentOptionSaveGame->SetGraphicsOptions(GraphicSubsys->GetCurrentOptions());
	}

	if (USoundSubsystem* SoundSubsys = GetGameInstance()->GetSubsystem<USoundSubsystem>())
	{
		CurrentOptionSaveGame->SetSoundOptions(SoundSubsys->GetCurrentOptions());
	}

	SaveOptions();
}

void UOptionManagerSubsystem::CreateNewOptionSaveData()
{
	CurrentOptionSaveGame = Cast<ULROptionSaveGame>(UGameplayStatics::CreateSaveGameObject(ULROptionSaveGame::StaticClass()));
	CurrentOptionSaveGame->ApplyDefaults();
}

void UOptionManagerSubsystem::InitializeOptions()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGraphicSubsystem* Subsys = GI->GetSubsystem<UGraphicSubsystem>())
		{
			Subsys->InitializeFromSaveData(CurrentOptionSaveGame->GetGraphicOptions());
		}

		if (USoundSubsystem* Subsys = GI->GetSubsystem<USoundSubsystem>())
		{
			Subsys->InitializeFromSaveData(CurrentOptionSaveGame->GetSoundOptions());
		}
	}
}

void UOptionManagerSubsystem::LoadOptions()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		CurrentOptionSaveGame = Cast<ULROptionSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
		if (!ensureMsgf(CurrentOptionSaveGame, TEXT("Could not load Game Option Save Slot")))
		{
			return;
		}
	}
	else
	{
		LR_INFO(TEXT("No Save Game Option found, creating new one"));
		CreateNewOptionSaveData();
		SaveOptions();
	}
}

void UOptionManagerSubsystem::SaveOptions()
{
	if (!ensureMsgf(CurrentOptionSaveGame, TEXT("CANNOT SAVE : CurrentOptionSaveGame is NULL")))
	{
		return;
	}

	if (UGameplayStatics::SaveGameToSlot(CurrentOptionSaveGame, SaveSlotName, UserIndex))
	{
		LR_INFO(TEXT("Save Game Option Successfully"));
	}
}
