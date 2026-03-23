// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Collection/LREnhanceButtonWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

void ULREnhanceButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonText = FText::Format(NSLOCTEXT("LREnhanceButtonWidget", "Text", "+{0} 강화"), FText::AsNumber(EnhanceCount));

	if (Text)
	{
		Text->SetText(ButtonText);
	}
}
