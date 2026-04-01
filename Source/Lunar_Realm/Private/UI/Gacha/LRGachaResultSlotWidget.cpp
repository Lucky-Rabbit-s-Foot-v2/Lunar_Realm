// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaResultSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"

namespace LRGachaResultSlotLocal
{
	static void SetFallbackName(UTextBlock* InTextBlock, FName InItemID)
	{
		if (InTextBlock)
		{
			InTextBlock->SetText(FText::FromName(InItemID));
		}
	}

	static void ClearIconBrush(UImage* InImage)
	{
		if (InImage)
		{
			InImage->SetBrushFromTexture(nullptr);
		}
	}
}

void ULRGachaResultSlotWidget::SetupWithResult(const FLRGachaResult& InResult)
{
	CachedResult = InResult;

	// 1) 이름 / 아이콘 세팅
	SetupNameAndIcon();

	// 2) 등급별 색상 적용
	const FLinearColor RarityColor = GetColorByRarity(CachedResult.Rarity);

	if (Image_RarityFrame)
	{
		Image_RarityFrame->SetColorAndOpacity(RarityColor);
	}

	if (Border_Background)
	{
		const FLinearColor BaseDark(0.1f, 0.1f, 0.1f, 1.f);
		const FLinearColor MixedColor = (RarityColor * 0.4f) + BaseDark;

		Border_Background->SetBrushColor(
			FLinearColor(
				FMath::Clamp(MixedColor.R, 0.0f, 1.0f),
				FMath::Clamp(MixedColor.G, 0.0f, 1.0f),
				FMath::Clamp(MixedColor.B, 0.0f, 1.0f),
				FMath::Clamp(MixedColor.A, 0.0f, 1.0f)
			)
		);
	}

	if (Image_NewBadge)
	{
		Image_NewBadge->SetVisibility(CachedResult.bIsNew ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

// 결과 슬롯이 화면에 추가된 직후 호출되는 등장 연출 시작점
void ULRGachaResultSlotWidget::PlayAppearEffect()
{
	// 1) BP 애니메이션 / BP 연출 실행
	BP_PlayAppearEffect(CachedResult);

	// 2) 등급별 사운드 선택
	USoundBase* PlaySound = nullptr;

	switch (CachedResult.Rarity)
	{
	case ELRGachaRarity::N:
		PlaySound = AppearSoundN;
		break;

	case ELRGachaRarity::R:
		PlaySound = AppearSoundR;
		break;

	case ELRGachaRarity::SR:
		PlaySound = AppearSoundSR;
		break;

	case ELRGachaRarity::SSR:
		PlaySound = AppearSoundSSR;
		break;

	case ELRGachaRarity::UR:
		PlaySound = AppearSoundUR;
		break;

	default:
		break;
	}

	// 3) 이전에 재생 중이던 슬롯 사운드가 있으면 먼저 정지
	StopAppearSound();

	// 4) 사운드 재생 (나중에 Stop 할 수 있게 AudioComponent 보관)
	if (PlaySound)
	{
		ActiveAppearSoundComponent = UGameplayStatics::SpawnSound2D(this, PlaySound);
	}
}

void ULRGachaResultSlotWidget::StopAppearSound()
{
	if (ActiveAppearSoundComponent)
	{
		ActiveAppearSoundComponent->Stop();
		ActiveAppearSoundComponent = nullptr;
	}
}

FLinearColor ULRGachaResultSlotWidget::GetColorByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:   return FLinearColor(0.65f, 0.65f, 0.65f, 1.f); // 회색
	case ELRGachaRarity::R:   return FLinearColor(0.55f, 1.00f, 0.35f, 1.f); // 연두색
	case ELRGachaRarity::SR:  return FLinearColor(0.15f, 0.45f, 1.00f, 1.f); // 파란색
	case ELRGachaRarity::SSR: return FLinearColor(0.60f, 0.20f, 0.90f, 1.f); // 보라색
	case ELRGachaRarity::UR:  return FLinearColor(1.00f, 0.78f, 0.10f, 1.f); // 황금색
	default:                  return FLinearColor::White;
	}
}

void ULRGachaResultSlotWidget::SetupNameAndIcon()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		LRGachaResultSlotLocal::SetFallbackName(Text_Name, CachedResult.ItemID);
		LRGachaResultSlotLocal::ClearIconBrush(Image_Icon);
		return;
	}

	UGameDataSubsystem* GameDataSys = GI->GetSubsystem<UGameDataSubsystem>();
	ULRGachaSubsystem* GachaSys = GI->GetSubsystem<ULRGachaSubsystem>();

	if (!GameDataSys)
	{
		LRGachaResultSlotLocal::SetFallbackName(Text_Name, CachedResult.ItemID);
		LRGachaResultSlotLocal::ClearIconBrush(Image_Icon);
		return;
	}

	UTexture2D* SlotTexture = nullptr;

	// 1) 가챠 전용 결과 슬롯 이미지 우선
	if (GachaSys)
	{
		SlotTexture = GachaSys->GetResultSlotTexture(CachedResult.ItemID, CachedResult.ItemType);
	}

	if (CachedResult.ItemType == ELRGachaItemType::Hero)
	{
		const FCharacterStaticData& CharData = GameDataSys->GetCharacterStaticData(CachedResult.ItemID);

		if (Text_Name)
		{
			Text_Name->SetText(
				!CharData.CharacterName.IsEmpty()
				? FText::FromString(CharData.CharacterName)
				: FText::FromName(CachedResult.ItemID)
			);
		}

		if (!SlotTexture)
		{
			if (!CharData.PortraitIcon.IsNull())
			{
				SlotTexture = CharData.PortraitIcon.LoadSynchronous();
			}
			else if (!CharData.CharacterTexture.IsNull())
			{
				SlotTexture = CharData.CharacterTexture.LoadSynchronous();
			}
		}
	}
	else
	{
		const FEquipmentStaticData& EquipData = GameDataSys->GetEquipmentStaticData(CachedResult.ItemID);

		if (Text_Name)
		{
			Text_Name->SetText(
				!EquipData.EquipmentName.IsEmpty()
				? FText::FromString(EquipData.EquipmentName)
				: FText::FromName(CachedResult.ItemID)
			);
		}

		if (!SlotTexture && !EquipData.EquipmentTexture.IsNull())
		{
			SlotTexture = EquipData.EquipmentTexture.LoadSynchronous();
		}
	}

	if (Image_Icon)
	{
		if (SlotTexture)
		{
			Image_Icon->SetBrushFromTexture(SlotTexture);
		}
		else
		{
			// 이전 슬롯 브러시가 남지 않도록 명시적으로 초기화
			Image_Icon->SetBrushFromTexture(nullptr);
		}
	}
}