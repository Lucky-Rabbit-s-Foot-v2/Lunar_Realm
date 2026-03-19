// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCharacterStatusWidget.h"

#include "Engine/GameInstance.h"

#include "Components/TextBlock.h"

#include "Data/LREnumType.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

void ULRCharacterStatusWidget::RefreshUI()
{
	Super::RefreshUI();

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	UCollectionSubsystem* CollectionSubsystem = GetGameInstance()->GetSubsystem<UCollectionSubsystem>();
	
	if (!GameDataSubsystem || !CollectionSubsystem)
	{
		return;
	}
	const FCharacterStaticData& CharacterStaticData = GameDataSubsystem->GetCharacterStaticData(CharacterID);
	const FCharacterInstance& CharacterInstance = CollectionSubsystem->GetCharacterInstance(CharacterID);

	float CharacterHPBonus = GameDataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::HP, 1);
	float CharacterATKBonus = GameDataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::ATK, 1);
	float CharacterDEFBonus = GameDataSubsystem->GetCharacterFinalStat(CharacterID, ELRStatusType::DEF, 1);
	
	Txt_Name->SetText(FText::FromString(CharacterStaticData.CharacterName));
	Txt_HP->SetText(FText::AsNumber(CharacterHPBonus));
	Txt_ATK->SetText(FText::AsNumber(CharacterATKBonus));
	Txt_DEF->SetText(FText::AsNumber(CharacterDEFBonus));

	float CurrentLevel = CharacterInstance.CurrentLevel;
	float CurrentExp = CharacterInstance.CurrentExp;

	// TODO: 레벨 구간 경험치 요구량 테이블 현재 없음. 향후 추가 예정.
	const float ExpToNextLevel = 500.0f;

	// Level과 경험치 표시 (예: "Lv. 5 (250/500 exp)")
	FString LevelText = FString::Printf(TEXT("Lv. %d (%.0f/%.0f exp)"), static_cast<int32>(CurrentLevel), CurrentExp, ExpToNextLevel);
	Txt_Level->SetText(FText::FromString(LevelText));
	Txt_Class->SetText(FText::FromString(GetClassNameByType(CharacterStaticData.ClassType)));
}

FString ULRCharacterStatusWidget::GetClassNameByType(ELRClassType ClassType) const
{
	switch (ClassType)
	{
	case ELRClassType::WARRIOR:
		return FString(TEXT("Warrior"));
	case ELRClassType::MAGICIAN:
		return FString(TEXT("Magician"));
	case ELRClassType::ARCHER:
		return FString(TEXT("Archer"));
	default:
		return FString();
	}
}
