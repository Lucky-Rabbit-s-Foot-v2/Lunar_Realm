// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterStatusWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "Engine/GameInstance.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

void ULRCharacterStatusWidget::SetCharacterID(const FName& InID)
{
	ID = InID;
	RefreshUI();
}

void ULRCharacterStatusWidget::RefreshUI()
{
	Super::RefreshUI();

	UpdateCharacterData();
	UpdateCharacterStatus();
}

void ULRCharacterStatusWidget::UpdateCharacterData()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();

	if (!GameDataSubsystem || !CollectionSubsystem)
	{
		return;
	}
	
	CharacterStaticData = GameDataSubsystem->GetCharacterStaticData(ID);
	CharacterInstance = CollectionSubsystem->GetCharacterInstance(ID);
}

void ULRCharacterStatusWidget::UpdateCharacterStatus()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	UpdateLevel();
	UpdateEXP();
	UpdateClass();
	UpdateHP();
	UpdateATK();
	UpdateDEF();
}

FString ULRCharacterStatusWidget::GetClassNameByType(ELRClassType ClassType) const
{
	switch (ClassType)
	{
	case ELRClassType::WARRIOR:		return FString(TEXT("전사"));
	case ELRClassType::MAGICIAN:	return FString(TEXT("마법사"));
	case ELRClassType::ARCHER:		return FString(TEXT("궁수"));
	default:						return FString();
	}
}

void ULRCharacterStatusWidget::UpdateLevel()
{
	int32 CurrentLevel = CharacterInstance.CurrentLevel;
	Level->SetText(FText::AsNumber(CurrentLevel));
}

void ULRCharacterStatusWidget::UpdateEXP()
{
	// TODO: 레벨 구간 경험치 요구량 테이블 현재 없음. 향후 추가 예정.
	const int32 ExpToNextLevel = 500;
	int32 CurrentExp = CharacterInstance.CurrentExp;

	float ExpProgress = (ExpToNextLevel > 0) ? static_cast<float>(CurrentExp) / ExpToNextLevel : 0.0f;
	Bar->SetPercent(ExpProgress);

	FString ExpText = FString::Printf(TEXT("%d / %d (%.1f%%)"), CurrentExp, ExpToNextLevel, ExpProgress * 100.0f);
	EXP->SetText(FText::FromString(ExpText));
}

void ULRCharacterStatusWidget::UpdateClass()
{
	FString CharacterClassName = GetClassNameByType(CharacterStaticData.ClassType);
	Class->SetText(FText::FromString(CharacterClassName));
}

void ULRCharacterStatusWidget::UpdateHP()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	float CharacterHPBonus = GameDataSubsystem->GetCharacterFinalStat(ID, ELRStatusType::HP, CharacterInstance.CurrentLevel);
	HP->SetText(FText::AsNumber(CharacterHPBonus));
}

void ULRCharacterStatusWidget::UpdateATK()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	float CharacterATKBonus = GameDataSubsystem->GetCharacterFinalStat(ID, ELRStatusType::ATK, CharacterInstance.CurrentLevel);
	ATK->SetText(FText::AsNumber(CharacterATKBonus));
}

void ULRCharacterStatusWidget::UpdateDEF()
{
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	float CharacterDEFBonus = GameDataSubsystem->GetCharacterFinalStat(ID, ELRStatusType::DEF, CharacterInstance.CurrentLevel);
	DEF->SetText(FText::AsNumber(CharacterDEFBonus));
}