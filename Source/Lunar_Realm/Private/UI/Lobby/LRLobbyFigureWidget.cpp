// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LRLobbyFigureWidget.h"

#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "TimerManager.h"

#include "Components/Button.h"
#include "Components/Image.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/GameDataSubsystem.h"

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
		Btn_Figure->OnClicked.Clear();
		Btn_Figure->OnHovered.Clear();
		Btn_Figure->OnUnhovered.Clear();
	}

	Super::UnbindProperties();
}

void ULRLobbyFigureWidget::RefreshUI()
{
	Super::RefreshUI();

	if (CurrentCharacterID.IsNone())
	{
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

void ULRLobbyFigureWidget::OnFigureClicked()
{
	// TODO: HUD 에서 도감으로 이동함.
	OnFigureClickedDel.Broadcast(CurrentCharacterID);
}

void ULRLobbyFigureWidget::OnFigureLongPressed()
{
	// TODO: 마우스 포인터를 관리하는 주체에서 피규어 정보 위젯 표시
	bIsLongPressTriggered = true;

	OnFigureLongPressedDel.Broadcast(CurrentCharacterID);
}

void ULRLobbyFigureWidget::OnFigureLongReleased()
{
	// TODO: 피규어 정보 위젯 숨김
	OnFigureLongReleasedDel.Broadcast();

}
