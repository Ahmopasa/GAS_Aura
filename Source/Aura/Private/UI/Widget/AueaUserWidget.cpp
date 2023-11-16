// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AueaUserWidget.h"

void UAueaUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
