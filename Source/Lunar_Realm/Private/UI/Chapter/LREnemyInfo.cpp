// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chapter/LREnemyInfo.h"

#include "Engine/Texture2D.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

void ULREnemyInfo::RefreshUI()
{
	Super::RefreshUI();

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const FEnemyStaticData& EnemyData = GameDataSubsystem->GetEnemyStaticData(EnemyID);

	UTexture2D* PortraitTexture = EnemyData.CharacterTexture.LoadSynchronous();
	Img_Portrait->SetBrushFromTexture(PortraitTexture);

	Txt_Name->SetText(FText::FromString(EnemyData.CharacterName));
}

void ULREnemyInfo::SetEnemyID(FName InEnemyID)
{
	EnemyID = InEnemyID;

	RefreshUI();
}
