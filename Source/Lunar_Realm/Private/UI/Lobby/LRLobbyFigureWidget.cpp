// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyFigureWidget.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "TimerManager.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"

#include "UI/Lobby/LRLobbyPageWidget.h"
#include "UI/Lobby/LRLobbyFigureInfoWidget.h"
#include "Units/OutGame/LROutGameController.h"

#include "Animation/WidgetAnimation.h"

void ULRLobbyFigureWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Figure)
	{
		Btn_Figure->OnPressed.AddUniqueDynamic(this, &ULRLobbyFigureWidget::OnFigurePressed);
		Btn_Figure->OnReleased.AddUniqueDynamic(this, &ULRLobbyFigureWidget::OnFigureReleased);
	}
}

void ULRLobbyFigureWidget::UnbindProperties()
{

	if (Btn_Figure)
	{
		Btn_Figure->OnPressed.Clear();
		Btn_Figure->OnReleased.Clear();
	}

	Super::UnbindProperties();
}

void ULRLobbyFigureWidget::RegisterSubWidgets()
{
	Super::RegisterSubWidgets();
	SubWidgets.Add(FigureInfoWidget);
}

void ULRLobbyFigureWidget::RefreshUI()
{
	Super::RefreshUI();
	
	if (SlotIndex < 0)
	{
		Img_Figure->SetBrushFromTexture(EmptySlotTexture);
		return;
	}

	USaveGameSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	FName CharacterID = SaveGameSubsystem->GetPartyCharacterID(SlotIndex);
	SetFigure(CharacterID);

	if (CurrentCharacterID.IsNone())
	{
		Img_Figure->SetBrushFromTexture(EmptySlotTexture);
		return;
	}

	UGameDataSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataSubsystem>();
	const FCharacterStaticData& StaticData = GameDataSubsystem->GetCharacterStaticData(CurrentCharacterID);
	if (Img_Figure)
	{
		Img_Figure->SetBrushFromTexture(StaticData.PortraitIcon.LoadSynchronous());
	}
}

void ULRLobbyFigureWidget::OnFigurePressed()
{
	bIsLongPressTriggered = false;

	GetWorld()->GetTimerManager().SetTimer(
		LongPressTimerHandle,
		this,
		&ULRLobbyFigureWidget::OnFigureLongPressed,
		0.3f,
		false
	);
}

void ULRLobbyFigureWidget::OnFigureReleased()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(LongPressTimerHandle))
	{
		OnFigureClicked();
	}
	else if (bIsLongPressTriggered)
	{
		// Long Press 후 릴리즈된 경우
		OnFigureLongReleased();
	}
	GetWorld()->GetTimerManager().ClearTimer(LongPressTimerHandle);
}

void ULRLobbyFigureWidget::SetFigure(FName CharacterID)
{
	CurrentCharacterID = CharacterID;
}

void ULRLobbyFigureWidget::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	FigureInfoWidget->SetSlotIndex(InSlotIndex);

	RefreshUI();
}

void ULRLobbyFigureWidget::OnFigureClicked()
{
	ALROutGameController* PC = Cast<ALROutGameController>(GetOwningPlayer());
	if (PC)
	{
		PC->OpenEnhancePage();
	}
	
	OnFigureClickedDel.Broadcast(SlotIndex);
}

void ULRLobbyFigureWidget::OnFigureLongPressed()
{
	bIsLongPressTriggered = true;

	OpenInfoWidget();
}

void ULRLobbyFigureWidget::OnFigureLongReleased()
{
	CloseInfoWidget();
}

void ULRLobbyFigureWidget::OpenInfoWidget()
{
	if (SlotIndex == 0)
	{
		if (Anim_HoverMain)
		{
			float CurrentTime = IsAnimationPlaying(Anim_HoverMain) ? GetAnimationCurrentTime(Anim_HoverMain) : 0.f;
			PlayAnimation(Anim_HoverMain, CurrentTime, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
	}
	else
	{
		if (Anim_Hover)
		{
			float CurrentTime = IsAnimationPlaying(Anim_Hover) ? GetAnimationCurrentTime(Anim_Hover) : 0.f;
			PlayAnimation(Anim_Hover, CurrentTime, 1, EUMGSequencePlayMode::Forward, 1.f);
		}
	}
}

void ULRLobbyFigureWidget::CloseInfoWidget()
{
	if (SlotIndex == 0)
	{
		if (Anim_HoverMain)
		{
			float CurrentTime = IsAnimationPlaying(Anim_HoverMain) ? GetAnimationCurrentTime(Anim_HoverMain) : Anim_HoverMain->GetEndTime();
			PlayAnimation(Anim_HoverMain, 0.f, 1, EUMGSequencePlayMode::Reverse, 1.f);
			SetAnimationCurrentTime(Anim_HoverMain, CurrentTime);
		}
	}
	else
	{
		if (Anim_Hover)
		{
			float CurrentTime = IsAnimationPlaying(Anim_Hover) ? GetAnimationCurrentTime(Anim_Hover) : Anim_Hover->GetEndTime();
			PlayAnimation(Anim_Hover, 0.f, 1, EUMGSequencePlayMode::Reverse, 1.f);
			SetAnimationCurrentTime(Anim_Hover, CurrentTime);
		}
	}
}
