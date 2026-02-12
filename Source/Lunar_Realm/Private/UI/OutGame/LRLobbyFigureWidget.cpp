// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRLobbyFigureWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "System/LoggingSystem.h"
#include "TimerManager.h"

#include "Components/Button.h"
#include "Components/Image.h"

void ULRLobbyFigureWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Figure)
	{
		Btn_Figure->OnPressed.AddDynamic(this, &ULRLobbyFigureWidget::OnFigurePressed);
		Btn_Figure->OnReleased.AddDynamic(this, &ULRLobbyFigureWidget::OnFigureReleased);
	}
}

void ULRLobbyFigureWidget::NativeDestruct()
{

	if (Btn_Figure)
	{
		Btn_Figure->OnClicked.Clear();
		Btn_Figure->OnHovered.Clear();
		Btn_Figure->OnUnhovered.Clear();
	}

	Super::NativeDestruct();
}

void ULRLobbyFigureWidget::OpenUI()
{
	Super::OpenUI();

	RefreshUI();
}

void ULRLobbyFigureWidget::RefreshUI()
{
	Super::RefreshUI();

	// TODO: 멤버가 바뀌면 Img_Figure 갱신
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
