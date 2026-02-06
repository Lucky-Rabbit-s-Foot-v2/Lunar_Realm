// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRPlayerWidget.h"

void ULRPlayerWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
