#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AueaAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AueaPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetAueaPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAueaASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag) 
		{
			auto Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	);

	GetAueaPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints) 
		{
			SpellPointsChanged.Broadcast(SpellPoints);
		}
	);
}
