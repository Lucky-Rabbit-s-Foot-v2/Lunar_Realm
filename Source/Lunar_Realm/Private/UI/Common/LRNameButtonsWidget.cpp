// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/LRNameButtonsWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"


void ULRNameButtonsWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Low) Btn_Low->OnClicked.AddDynamic(this, &ULRNameButtonsWidget::OnLowButtonClicked);
	if (Btn_Medium) Btn_Medium->OnClicked.AddDynamic(this, &ULRNameButtonsWidget::OnMediumButtonClicked);
	if (Btn_High) Btn_High->OnClicked.AddDynamic(this, &ULRNameButtonsWidget::OnHighButtonClicked);
}

void ULRNameButtonsWidget::UnbindProperties()
{
	Btn_High->OnClicked.Clear();
	Btn_Medium->OnClicked.Clear();
	Btn_Low->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRNameButtonsWidget::SetName(const FText& Name)
{
	if(Txt_Name)
	{
		Txt_Name->SetText(Name);
	}
}

void ULRNameButtonsWidget::OnLowButtonClicked()
{
	OnButtonClickedDel.Broadcast(0);
}

void ULRNameButtonsWidget::OnMediumButtonClicked()
{
	OnButtonClickedDel.Broadcast(1);
}

void ULRNameButtonsWidget::OnHighButtonClicked()
{
	OnButtonClickedDel.Broadcast(2);
}
