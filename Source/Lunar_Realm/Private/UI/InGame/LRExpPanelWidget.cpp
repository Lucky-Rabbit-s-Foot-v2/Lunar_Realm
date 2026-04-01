// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRExpPanelWidget.h"
#include "UI/InGame/LRExpSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"
#include "Subsystems/StageManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Components/VerticalBoxSlot.h"

void ULRExpPanelWidget::SetupExpPanel(bool bInIsClear, int32 InTargetGroup)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	USaveGameSubsystem* SaveSys = GI->GetSubsystem<USaveGameSubsystem>();
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	UCollectionSubsystem* CollectionSys = GI->GetSubsystem<UCollectionSubsystem>();
	UStageManagerSubsystem* StageSys = GI->GetSubsystem<UStageManagerSubsystem>();

	if (!SaveSys || !DataSys || !CollectionSys || !StageSys) return;

	// 스테이지 기본 보상 경험치 획득
	const FStageStaticData* StageData = StageSys->GetCurrentStageData();
	if (!StageData)
	{
		LR_WARN(TEXT("현재 스테이지 데이터를 찾을 수 없습니다!"));
		return;
	}
	FName CurrentStageID = StageData->DataID;
	int32 BaseExp = StageData->RewardExp;

	// 클리어 여부 확인
	FStageClearedData ClearData = SaveSys->GetStageClearedData(CurrentStageID);
	bool bAlreadyCleared = (ClearData.StarMasking > 0);
	
	float FinalMultiplier = 1.0f;

	if (bInIsClear)
	{
		// 승리 시
		FinalMultiplier = bAlreadyCleared ? 0.75f : 1.0f;

		if (NoticeText)
		{
			if (bAlreadyCleared)
			{
				NoticeText->SetText(FText::FromString(TEXT("완료된 스테이지는\n75 % 경험치만 제공합니다.")));
				NoticeText->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				NoticeText->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	else
	{
		// 패배 시 (20% 지급)
		FinalMultiplier = 0.2f;

		if (NoticeText)
		{
			NoticeText->SetText(FText::FromString(TEXT("전투 패배 보상으로\n20 % 경험치를 획득합니다.")));
			NoticeText->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// 경험치 분배
	int32 PlayerExp = FMath::RoundToInt(BaseExp * FinalMultiplier);
	int32 MemberExp = FMath::RoundToInt(PlayerExp * 0.7f);


	// 컨테이너 초기화
	if (LeaderSlotContainer) LeaderSlotContainer->ClearChildren();
	if (MemberSlotContainer) MemberSlotContainer->ClearChildren();
	SubWidgets.Empty();

	if (!ExpSlotClass)
	{
		LR_ERROR(TEXT("ExpSlotClass가 할당되지 않았음."));
		return;
	}

	// 파티 멤버 슬롯 생성 및 경험치 분배 로직
	TArray<FName> PartyIDs = SaveSys->GetAllPartyCharactersIDs();
	for (int32 i = 0; i < PartyIDs.Num(); ++i)
	{

		if (InTargetGroup == 1 && i != 0) continue;
		if (InTargetGroup == 2 && i == 0) continue;

		FName CharID = PartyIDs[i];
		if (CharID == NAME_None) continue;

		// 0번은 플레이어, 나머지는 멤버
		int32 GainedExp = (i == 0) ? PlayerExp : MemberExp;

		// CollectionSubsystem에서 현재 레벨과 경험치 가져오기
		FCharacterInstance CharInst = CollectionSys->GetCharacterInstance(CharID);
		int32 OldLevel = CharInst.CurrentLevel;
		int32 OldExp = CharInst.CurrentExp;

		ULRExpSlotWidget* SlotWidget = CreateWidget<ULRExpSlotWidget>(this, ExpSlotClass);
		if (SlotWidget)
		{
			SlotWidget->SetSlotInfo(CharID, OldLevel, OldExp, GainedExp);

			if (i == 0)
			{
				if (LeaderSlotContainer)
				{
					UVerticalBoxSlot* LeaderSlot = LeaderSlotContainer->AddChildToVerticalBox(SlotWidget);
					if (LeaderSlot)
					{
						LeaderSlot->SetVerticalAlignment(VAlign_Center);
					}
				}
			}
			else
			{
				if (MemberSlotContainer)
				{
					UVerticalBoxSlot* MemberSlot = MemberSlotContainer->AddChildToVerticalBox(SlotWidget);
					if (MemberSlot)
					{
						MemberSlot->SetVerticalAlignment(VAlign_Center);

						// 영역 안에서 꽉 채우게 분배하고 싶다면
						// MemberSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
					}
				}
			}

			SubWidgets.Add(SlotWidget);
		}
		CollectionSys->AddCharacterExp(CharID, GainedExp);
	}
}

