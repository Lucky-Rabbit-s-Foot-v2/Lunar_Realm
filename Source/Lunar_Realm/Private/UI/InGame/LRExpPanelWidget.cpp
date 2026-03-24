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

void ULRExpPanelWidget::SetupExpPanel()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	USaveGameSubsystem* SaveSys = GI->GetSubsystem<USaveGameSubsystem>();
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	UCollectionSubsystem* CollectionSys = GI->GetSubsystem<UCollectionSubsystem>();
	UStageManagerSubsystem* StageSys = GI->GetSubsystem<UStageManagerSubsystem>();

	if (!SaveSys || !DataSys || !CollectionSys || !StageSys) return;

	// 스테이지 기본 보상 경험치 획득
	const FStageStaticData* StageData = StageSys->GetCurrentStateData();
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

	// 패널티 및 경험치 분배 로직
	float FinalMultiplier = bAlreadyCleared ? 0.75f : 1.0f;
	int32 PlayerExp = FMath::RoundToInt(BaseExp * FinalMultiplier);
	int32 MemberExp = FMath::RoundToInt(PlayerExp * 0.7f);

	// 안내 문구 텍스트 세팅
	if (NoticeText)
	{
		if (bAlreadyCleared)
		{
			NoticeText->SetText(FText::FromString(TEXT("완료한 스테이지라 경험치가 75%만 지급됩니다.")));
			NoticeText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			NoticeText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (!SlotContainer)
	{
		LR_ERROR(TEXT("SlotContainer가 바인딩되지 않았습니다! BP_ExpPanelWidget을 확인하세요."));
		return;
	}

	// 컨테이너 초기화
	SlotContainer->ClearChildren();
	SubWidgets.Empty();

	if (!ExpSlotClass) return;

	// 파티 멤버 슬롯 생성 및 경험치 분배 로직
	TArray<FName> PartyIDs = SaveSys->GetAllPartyCharactersIDs();
	for (int32 i = 0; i < PartyIDs.Num(); ++i)
	{
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

			SlotContainer->AddChildToVerticalBox(SlotWidget);
			SubWidgets.Add(SlotWidget);
		}

		// 애니메이션 세팅이 끝났으니, 이제 실제로 DB에 경험치 저장
		CollectionSys->AddCharacterExp(CharID, GainedExp);
	}
}
