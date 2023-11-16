// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AueaWidgetController.h"

void UAueaWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAueaWidgetController::BroadcastInitialValues()
{

}

void UAueaWidgetController::BindCallbacksToDependencies()
{

}
