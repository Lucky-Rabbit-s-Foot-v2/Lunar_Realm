// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LRCollectionPageWidget.h"

#include "Components/Button.h"

#include "Engine/GameInstance.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "UI/Collection/LRCollection.h"

void ULRCollectionPageWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Back) Btn_Back->OnClicked.AddDynamic(this, &ULRCollectionPageWidget::OnBackButtonClicked);
}

void ULRCollectionPageWidget::UnbindProperties()
{
	if (Btn_Back) Btn_Back->OnClicked.Clear();

	Super::UnbindProperties();
}

void ULRCollectionPageWidget::RegisterSubWidgets()
{
	SubWidgets.Add(CharacterInfo);
	SubWidgets.Add(Collection);
}

void ULRCollectionPageWidget::OnBackButtonClicked()
{
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->OpenUIByID(EUIID::LOBBY);
	}
}
