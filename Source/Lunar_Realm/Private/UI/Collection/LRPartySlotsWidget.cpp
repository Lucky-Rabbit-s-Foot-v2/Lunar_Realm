// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartySlotsWidget.h"

#include "Components/Image.h"

#include "Engine/Texture2D.h"
#include "Engine/GameInstance.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Subsystems/CollectionSubsystem.h"

#include "UI/Collection/LRPartySlotWidget.h"
#include "UI/Collection/LRPartyCharacterSlot.h"
#include "UI/Collection/LRPartyEquipmentSlot.h"

#include "UI/Collection/LREnhancePageWidget.h"

#include "UI/Core/LRButtonWidget.h"

#include "Units/OutGame/LROutGameController.h"

void ULRPartySlotsWidget::InitializeUI()
{
	Super::InitializeUI();

	Slot0->SetSlotIndex(0);
	Slot1->SetSlotIndex(1);
	Slot2->SetSlotIndex(2);
	Slot3->SetSlotIndex(3);
	Slot4->SetSlotIndex(4);

	SetEnableButtons(false);
}

void ULRPartySlotsWidget::BindProperties()
{
	Super::BindProperties();
	
	Btn_Enhance->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::OnPartyEnhanceClicked);
	Btn_Release->OnLRButtonClickedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::OnPartyReleaseClicked);
}

void ULRPartySlotsWidget::UnbindProperties()
{
	Btn_Enhance->OnLRButtonClickedDel.RemoveDynamic(this, &ULRPartySlotsWidget::OnPartyEnhanceClicked);
	Btn_Release->OnLRButtonClickedDel.RemoveDynamic(this, &ULRPartySlotsWidget::OnPartyReleaseClicked);
	
	Super::UnbindProperties();
}

void ULRPartySlotsWidget::BindToController(ALRControllerBase* Controller)
{
	Super::BindToController(Controller);

	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::SetIDAndType);
		PC->OnButtonVisibleDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::SetEnableButtons);
	}
}

void ULRPartySlotsWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();

	SubWidgets.Add(Slot0);
	SubWidgets.Add(Slot1);
	SubWidgets.Add(Slot2);
	SubWidgets.Add(Slot3);
	SubWidgets.Add(Slot4);
}

void ULRPartySlotsWidget::RefreshUI()
{
	Super::RefreshUI();
	RefreshCurrentImage();
}

void ULRPartySlotsWidget::SetIDAndType(FName InID, ECollectionType InType)
{
	ID = InID;
	Type = InType;

	RefreshUI();
}

void ULRPartySlotsWidget::RefreshCurrentImage()
{
	if (ID.IsNone())
	{
		Img_Current->SetBrushFromTexture(EmptySlotTexture);
		return;
	}

	switch (Type)
	{
		case ECollectionType::CHARACTER:
		{
			UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
			const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(ID);
			Img_Current->SetBrushFromTexture(StaticData.WholeBodyImage.LoadSynchronous());
			break;
		}
		
		case ECollectionType::EQUIPMENT:
		{
			UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
			const FEquipmentStaticData& StaticData = GameDataSubsystem->GetEquipmentStaticData(ID);
			Img_Current->SetBrushFromTexture(StaticData.EquipmentTexture.LoadSynchronous());
			break;
		}

		default:
			Img_Current->SetBrushFromTexture(EmptySlotTexture);
			break;
	}
}

void ULRPartySlotsWidget::OnPartyEnhanceClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->OpenEnhancePage();

		SetEnableButtons(false);
	}
}

void ULRPartySlotsWidget::OnPartyReleaseClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->ReleasePartySlot();

		SetEnableButtons(false);
	}
}

void ULRPartySlotsWidget::SetEnableButtons(bool bIsEnable)
{
	Btn_Enhance->SetIsEnabled(bIsEnable);
	Btn_Enhance->SetRenderOpacity(bIsEnable ? 1.0f : 0.3f);

	Btn_Release->SetIsEnabled(bIsEnable);
	Btn_Release->SetRenderOpacity(bIsEnable ? 1.0f : 0.3f);
}
