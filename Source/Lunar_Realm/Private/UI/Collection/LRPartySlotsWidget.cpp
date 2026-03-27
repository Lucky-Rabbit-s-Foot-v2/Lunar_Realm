// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRPartySlotsWidget.h"

#include "Components/Image.h"

#include "Engine/Texture2D.h"
#include "Engine/GameInstance.h"

#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Collection/LRPartySlotWidget.h"
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
		PC->OnSelectedChangedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::SetCurrentImage);
		PC->OnSlotSelectedDel.AddUniqueDynamic(this, &ULRPartySlotsWidget::SetEnableButtons);
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

void ULRPartySlotsWidget::SetCurrentImage(const FSelectedInfo& InInfo)
{
	if(InInfo.ID.IsNone() || InInfo.Type != ECollectionType::CHARACTER)
	{
		Img_Current->SetBrushFromTexture(EmptySlotTexture);
		return;
	}
	FName CurrentID = InInfo.ID;
	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(CurrentID);
	Img_Current->SetBrushFromTexture(StaticData.WholeBodyImage.LoadSynchronous());
}

void ULRPartySlotsWidget::OnPartyEnhanceClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		PC->OpenEnhancePage();
	}
}

void ULRPartySlotsWidget::OnPartyReleaseClicked()
{
	if (ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer()))
	{
		const FSelectedInfo& SelectedInfo = PC->GetSelectedInfo();

		ECollectionType Type = SelectedInfo.Type;
		int32 SlotIndex = SelectedInfo.SlotIndex;
		
		USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();

		if (Type == ECollectionType::EQUIPMENT)
		{
			SaveGameSubsystem->SetLeaderEquipmentSlot(SlotIndex, FGuid());
		}
		if (Type == ECollectionType::CHARACTER)
		{
			SaveGameSubsystem->SetPartySlot(SlotIndex, NAME_None);
		}
		PC->ResetSelectedInfo();
	}
}

void ULRPartySlotsWidget::SetEnableButtons(bool bIsEnable)
{
	Btn_Enhance->SetIsEnabled(bIsEnable);
	Btn_Enhance->SetRenderOpacity(bIsEnable ? 1.0f : 0.3f);

	Btn_Release->SetIsEnabled(bIsEnable);
	Btn_Release->SetRenderOpacity(bIsEnable ? 1.0f : 0.3f);
}
