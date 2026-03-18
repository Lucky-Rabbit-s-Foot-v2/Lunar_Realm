// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Core/LRButtonWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void ULRButtonWidget::BindProperties()
{
	Super::BindProperties();
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &ULRButtonWidget::OnButtonClicked);
	}
}

void ULRButtonWidget::OnButtonClicked()
{

}