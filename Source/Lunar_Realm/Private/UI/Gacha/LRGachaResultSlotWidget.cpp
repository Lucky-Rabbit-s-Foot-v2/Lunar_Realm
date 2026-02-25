// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaResultSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"

void ULRGachaResultSlotWidget::SetupWithResult(const FLRGachaResult& InResult)
{
	CachedResult = InResult;

	// 1) 이름 / 아이콘 세팅
	SetupNameAndIcon();

	// 2) 등급별 색상 적용
	const FLinearColor RarityColor = GetColorByRarity(CachedResult.Rarity);

	if (Image_RarityFrame)
	{
		// 프레임 TintColor만 변경
		Image_RarityFrame->SetColorAndOpacity(RarityColor);
	}

	if (Border_Background)
	{
		// 배경은 등급색 + 어두운 톤 섞어서 살짝 그라데이션 느낌
		const FLinearColor BaseDark(0.1f, 0.1f, 0.1f, 1.f);
		Border_Background->SetBrushColor(RarityColor * 0.4f + BaseDark);
	}
}

FLinearColor ULRGachaResultSlotWidget::GetColorByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::Common:    return FLinearColor(0.8f, 0.8f, 0.8f, 1.f);   // 흰/회색
	case ELRGachaRarity::Elite:     return FLinearColor(0.1f, 0.4f, 1.0f, 1.f);   // 파랑
	case ELRGachaRarity::Unique:    return FLinearColor(0.9f, 0.1f, 0.1f, 1.f);   // 빨강
	case ELRGachaRarity::Epic:      return FLinearColor(0.5f, 0.1f, 0.9f, 1.f);   // 보라
	case ELRGachaRarity::Legendary: return FLinearColor(1.0f, 0.75f, 0.0f, 1.f);  // 금색
	default:                        return FLinearColor::White;
	}
}

void ULRGachaResultSlotWidget::SetupNameAndIcon()
{
	// 지금은 간단히 ItemID 기반 이름만 표시.
	// 나중에 GameDataSubsystem에서 StaticData를 가져와
	// 한글 이름 / 아이콘 Texture를 세팅하는 식으로 확장.

	// 이름
	if (Text_Name)
	{
		// 임시: ItemID를 문자열로 노출
		Text_Name->SetText(FText::FromName(CachedResult.ItemID));
	}

	// 아이콘
	if (Image_Icon)
	{
		// TODO:
		//  GameDataSubsystem에서 Hero/Equip StaticData를 가져와서
		//  UTexture2D* IconTexture 를 얻은 뒤
		//  Image_Icon->SetBrushFromTexture(IconTexture); 호출.
		//
		// 지금은 아이콘이 없으므로 기본/투명 상태 유지.
	}
}