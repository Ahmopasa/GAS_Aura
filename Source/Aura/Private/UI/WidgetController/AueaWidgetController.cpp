// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AueaWidgetController.h"
#include "Player/AueaPlayerController.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/AueaAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

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

void UAueaWidgetController::BroadcastAbilityInfo()
{
	if (!GetAueaASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec) {
			auto Info = AbilityInfo->FindAbilityInfoForTag(GetAueaASC()->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = GetAueaASC()->GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = GetAueaASC()->GetStatusFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	GetAueaASC()->ForEachAbility(BroadcastDelegate);
}

AAueaPlayerController* UAueaWidgetController::GetAueaPC()
{
	return !AueaPlayerController ? AueaPlayerController = Cast<AAueaPlayerController>(PlayerController) : AueaPlayerController;
}

AAueaPlayerState* UAueaWidgetController::GetAueaPS()
{
	return !AueaPlayerState ? AueaPlayerState= Cast<AAueaPlayerState>(PlayerState) : AueaPlayerState;
}

UAueaAbilitySystemComponent* UAueaWidgetController::GetAueaASC()
{
	return !AueaAbilitySystemComponent ? AueaAbilitySystemComponent = Cast<UAueaAbilitySystemComponent>(AbilitySystemComponent) : AueaAbilitySystemComponent;
}

UAueaAttributeSet* UAueaWidgetController::GetAueaAS()
{
	return !AueaAttributeSet ? AueaAttributeSet = Cast<UAueaAttributeSet>(AttributeSet) : AueaAttributeSet;
}
