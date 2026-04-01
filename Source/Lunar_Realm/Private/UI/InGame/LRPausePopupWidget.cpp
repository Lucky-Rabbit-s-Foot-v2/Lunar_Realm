// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPausePopupWidget.h"

#include "Core/LRGameInstance.h"
#include "Core/Stage/LRStageGameMode.h"

#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Core/LRButtonWidget.h"

#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Data/LRGameDataConfig.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void ULRPausePopupWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Restart) Btn_Restart->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnRestartButtonClicked);
	if (Btn_Resume) Btn_Resume->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnResumeButtonClicked);
	if (Btn_Setting) Btn_Setting->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnSettingButtonClicked);
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPausePopupWidget::OnExitButtonClicked);

	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		OnCloseUIRequestedDel.AddUniqueDynamic(UIManager, &UUIManagerSubsystem::CloseUI);
	}
}

void ULRPausePopupWidget::UnbindProperties()
{
	if (Btn_Restart) Btn_Restart->OnLRButtonClickedDel.Clear();
	if (Btn_Resume) Btn_Resume->OnLRButtonClickedDel.Clear();
	if (Btn_Setting) Btn_Setting->OnLRButtonClickedDel.Clear();
	if (Btn_Exit) Btn_Exit->OnLRButtonClickedDel.Clear();

	Super::UnbindProperties();
}

void ULRPausePopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdatePauseUI();

}

void ULRPausePopupWidget::OnRestartButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnRestartGame();
	}
}

void ULRPausePopupWidget::OnResumeButtonClicked()
{
	OnCloseUIRequestedDel.Broadcast(this);
	
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnResumeGame();
	}
}

void ULRPausePopupWidget::OnSettingButtonClicked()
{
	if(UUIManagerSubsystem * UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::SETTING);
	}
}

void ULRPausePopupWidget::OnExitButtonClicked()
{
	if (ALRStageGameMode* StageGM = Cast<ALRStageGameMode>(GetWorld()->GetAuthGameMode()))
	{
		StageGM->OnExitStage();
	}
}

void ULRPausePopupWidget::UpdatePauseUI()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	USaveGameSubsystem* SaveSys = GI->GetSubsystem<USaveGameSubsystem>();
	UStageManagerSubsystem* StageSys = GI->GetSubsystem<UStageManagerSubsystem>();
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();

	if (!SaveSys || !StageSys || !DataSys) return;

	// 스테이지 및 챕터 세팅
	const FStageStaticData* StageData = StageSys->GetCurrentStageData();
	if (StageData)
	{
		if (Txt_StageName)Txt_StageName->SetText(StageData->StageName);
		
		// TODO_BJM : 챕터 이름 받아오면 주석풀기
		//if (Txt_ChapterName)Txt_ChapterName->SetText(ChapterData->ChapterName);
		
	}

	// 플레이어 덱 아이콘 세팅
	TArray<FName> PartyIDs = SaveSys->GetAllPartyCharactersIDs();
	TArray<UImage*> DeckImages = { Img_DeckLeader, Img_DeckMember1, Img_DeckMember2, Img_DeckMember3, Img_DeckMember4 };

	for (int32 i = 0; i < DeckImages.Num(); ++i)
	{
		if (!DeckImages[i]) continue;

		if(PartyIDs.IsValidIndex(i) && PartyIDs[i] != NAME_None)
		{
			const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(PartyIDs[i]);
			
			if (UTexture2D* LoadedTex = CharData.CharacterTexture.LoadSynchronous())
			{
				DeckImages[i]->SetBrushFromTexture(LoadedTex);
				DeckImages[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				DeckImages[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			DeckImages[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 적 아이콘 세팅

	TArray<FName> UniqueEnemyIDs;

	const FStageSpawnerData& SpawnerData = StageSys->GetCurrentStageSpawnerData();
	for (const FStageSpawnEnemyData& EnemySpawnData : SpawnerData.SpawnableEnemies)
	{
		UniqueEnemyIDs.AddUnique(EnemySpawnData.EnemyID);
	}

	if (StageSys->IsBossStage())
	{
		UniqueEnemyIDs.AddUnique(StageSys->GetBossEnemyID());
	}

	TArray<UImage*> EnemyImages = { Img_Enemy1, Img_Enemy2, Img_Enemy3 };

	for (int32 i = 0; i < EnemyImages.Num(); ++i)
	{
		if (!EnemyImages[i]) continue;

		if (UniqueEnemyIDs.IsValidIndex(i) && UniqueEnemyIDs[i] != NAME_None)
		{
			const FEnemyStaticData& EnemyData = DataSys->GetEnemyStaticData(UniqueEnemyIDs[i]);

			if (UTexture2D* LoadedEnemyTex = EnemyData.CharacterTexture.LoadSynchronous())
			{
				EnemyImages[i]->SetBrushFromTexture(LoadedEnemyTex);
				EnemyImages[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				EnemyImages[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
		else
		{
			EnemyImages[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 보상 정보 세팅
	const FStageRewardData& RewardData = StageSys->GetCurrentStageRewardData();

	if (Txt_RewardGold)
	{
		Txt_RewardGold->SetText(FText::AsNumber(RewardData.Gold));
	}

	if (Txt_RewardNormalTicket)
	{
		Txt_RewardNormalTicket->SetText(FText::AsNumber(RewardData.NormalTicket));
	}

	if (Txt_RewardEnhanceTicket)
	{
		Txt_RewardEnhanceTicket->SetText(FText::AsNumber(RewardData.EnhanceTicket));
	}
}