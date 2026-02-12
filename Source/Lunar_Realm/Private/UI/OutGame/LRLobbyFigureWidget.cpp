// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OutGame/LRLobbyFigureWidget.h"

#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Components/Button.h"
#include "Components/Image.h"

void ULRLobbyFigureWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Figure)
	{
		Btn_Figure->OnClicked.AddDynamic(this, &ULRLobbyFigureWidget::OnFigureButtonClicked);
		Btn_Figure->OnHovered.AddDynamic(this, &ULRLobbyFigureWidget::OnFigureButtonHovered);
		Btn_Figure->OnUnhovered.AddDynamic(this, &ULRLobbyFigureWidget::OnFigureButtonUnhovered);
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

void ULRLobbyFigureWidget::OnFigureButtonClicked()
{
	// TODO: HUD 에서 도감으로 이동함.
	OnFigureClickedDel.Broadcast(CurrentCharacterID);
}

void ULRLobbyFigureWidget::OnFigureButtonHovered()
{
	// TODO: 마우스 포인터를 관리하는 주체에서 피규어 정보 위젯 표시
	OnFigureHoveredDel.Broadcast(CurrentCharacterID);
}

void ULRLobbyFigureWidget::OnFigureButtonUnhovered()
{
	// TODO: 피규어 정보 위젯 숨김
	OnFigureUnhoveredDel.Broadcast();
}
