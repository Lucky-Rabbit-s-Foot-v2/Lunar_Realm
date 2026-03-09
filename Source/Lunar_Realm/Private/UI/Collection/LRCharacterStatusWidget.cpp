// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterStatusWidget.h"

#include "Engine/GameInstance.h"

#include "Components/TextBlock.h"

#include "Data/LREnumType.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

void ULRCharacterStatusWidget::RefreshUI()
{
	Super::RefreshUI();

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	if(GameDataSubsystem == nullptr || SaveGameSubsystem == nullptr)
	{
		LR_ERROR(TEXT("GameDataSubsystem or SaveGameSubsystem is not available"));
		return;
	}
	const FCharacterStaticData& CharacterStaticData = GameDataSubsystem->GetCharacterStaticData(CharacterID);

	float CharacterHPBonus = GameDataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::HP, 1);
	float CharacterATKBonus = GameDataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::ATK, 1);
	float CharacterDEFBonus = GameDataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::DEF, 1);
	
	// TODO: 캐릭터 레벨 변동 시 추가 작업 필요
	// TODO: 장비 보너스 적용 필요

	Txt_Name->SetText(FText::FromString(CharacterStaticData.CharacterName));
	Txt_HP->SetText(FText::AsNumber(CharacterHPBonus));
	Txt_ATK->SetText(FText::AsNumber(CharacterATKBonus));
	Txt_DEF->SetText(FText::AsNumber(CharacterDEFBonus));
}